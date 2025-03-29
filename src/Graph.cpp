/**
 * @file Graph.cpp
 * @brief Implementation of the Graph class with secure and hardware-aware edge list parsing.
 */

#include "Graph.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>
#include <immintrin.h> // For SIMD vectorization
#include <sys/mman.h>  // For memory-mapped file
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#include <omp.h>
#include <limits>
#include <filesystem>
#include "absl/container/flat_hash_set.h"

const std::vector<int> Graph::static_empty_vector;

// AVX2-optimized digit scanner
static const char* find_next_digit(const char* p, const char* end) {
    constexpr int simd_size = 32;
    if (end - p < simd_size) {
        while (p < end && (*p < '0' || *p > '9')) ++p;
        return p;
    }

    __m256i v_0 = _mm256_set1_epi8('0' - 1);
    __m256i v_9 = _mm256_set1_epi8('9' + 1);

    while (p + simd_size <= end) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p));
        __m256i cmp_gt = _mm256_cmpgt_epi8(chunk, v_0);
        __m256i cmp_lt = _mm256_cmpgt_epi8(v_9, chunk);
        int mask = _mm256_movemask_epi8(_mm256_and_si256(cmp_gt, cmp_lt));
        
        if (mask != 0) {
            return p + __builtin_ctz(mask);
        }
        p += simd_size;
    }

    while (p < end && (*p < '0' || *p > '9')) ++p;
    return p;
}

bool Graph::validate_filepath(const std::string& path) const {
    try {
        // Resolve absolute path and check file existence
        auto abs_path = std::filesystem::canonical(path);
        
        // Additional security checks
        if (!std::filesystem::is_regular_file(abs_path)) return false;
        if (abs_path.string().find("..") != std::string::npos) return false;
        
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

void Graph::load_graph(const std::string& filepath) {
    if (!validate_filepath(filepath)) throw std::runtime_error("Invalid path");

    int fd = open(filepath.c_str(), O_RDONLY);
    if (fd == -1) throw std::runtime_error("File open failed");

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        close(fd);
        throw std::runtime_error("File stat failed");
    }
    size_t file_size = sb.st_size;

    char* data = static_cast<char*>(mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0));
    close(fd);
    if (data == MAP_FAILED) throw std::runtime_error("mmap failed");

    std::vector<int> numbers;
    numbers.reserve(file_size / 3);

    const char* p = data;
    const char* end = data + file_size;
    
    // SIMD-accelerated number parsing
    while (p < end) {
        p = find_next_digit(p, end);
        if (p >= end) break;

        int num = 0;
        while (p < end && *p >= '0' && *p <= '9') {
            if (num > (std::numeric_limits<int>::max() - (*p - '0')) / 10) {
                munmap(data, file_size);
                throw std::runtime_error("Integer overflow detected");
            }
            num = num * 10 + (*p++ - '0');
        }
        numbers.push_back(num);
    }

    // Parallel processing with thread-local storage
    std::vector<absl::flat_hash_map<int, std::vector<int>>> thread_adj_lists(omp_get_max_threads());
    
    #pragma omp parallel for schedule(static)
    for (size_t i = 0; i < numbers.size(); i += 2) {
        if (i+1 >= numbers.size()) continue;
        
        int src = numbers[i];
        int dst = numbers[i+1];
        auto& local_adj = thread_adj_lists[omp_get_thread_num()];
        local_adj[src].emplace_back(dst);
        local_adj[dst].emplace_back(src);
    }

    // Atomic merge with copy-on-write
    {
        std::unique_lock lock(adj_list_mutex);
        for (auto& local_adj : thread_adj_lists) {
            for (auto& [node, neighbors] : local_adj) {
                auto new_neighbors = std::make_shared<const std::vector<int>>(std::move(neighbors));
                adj_list.insert_or_assign(node, new_neighbors);
            }
        }
    }

    munmap(data, file_size);
}

const std::vector<int>& Graph::get_neighbors(int node) const {
    std::shared_ptr<const std::vector<int>> result;
    {
        std::shared_lock lock(adj_list_mutex);
        auto it = adj_list.find(node);
        result = (it != adj_list.end()) ? it->second : nullptr;
    }
    return result ? *result : static_empty_vector;
}

void Graph::add_node(int node) {
    std::unique_lock lock(adj_list_mutex);
    adj_list.try_emplace(node);
}

void Graph::delete_node(int node) {
    std::unique_lock lock(adj_list_mutex);
    auto it = adj_list.find(node);
    if (it == adj_list.end()) return;

    // Get actual vector reference
    const auto& neighbors = *it->second;  // Dereference here
    
    for (int neighbor : neighbors) {
        if (auto nit = adj_list.find(neighbor); nit != adj_list.end()) {
            const auto& old_list = *nit->second;  // Dereference here
            std::vector<int> new_list;
            new_list.reserve(old_list.size());
            std::copy_if(old_list.begin(), old_list.end(), std::back_inserter(new_list),
                [node](int x) { return x != node; });
            adj_list.insert_or_assign(neighbor, 
                std::make_shared<const std::vector<int>>(new_list));
        }
    }
    
    adj_list.erase(it);
}

void Graph::save_graph(const std::string& filename) const {
    if (!validate_filepath(filename)) throw std::runtime_error("Invalid path");

    std::ofstream outfile(filename);
    if (!outfile.is_open()) throw std::runtime_error("Failed to open output file");

    absl::flat_hash_set<std::pair<int, int>, PairHash> written_edges;
    
    {
        std::shared_lock lock(adj_list_mutex);
        for (const auto& [src, neighbors_ptr] : adj_list) {
            for (int dst : *neighbors_ptr) {
                auto edge = std::minmax(src, dst);
                if (written_edges.insert(edge).second) {
                    outfile << edge.first << ' ' << edge.second << '\n';
                }
            }
        }
    }
    
    outfile.close();
}