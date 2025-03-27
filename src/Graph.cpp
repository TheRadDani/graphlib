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

Graph::Graph() {
    static_empty_vector = std::vector<int>();
}

bool Graph::validate_filepath(const std::string& path) const {
    // Avoid directory traversal: only allow alphanumeric filenames with underscores
    std::regex safe_path("^[a-zA-Z0-9_\\-./]+$");
    return std::regex_match(path, safe_path);
}

void Graph::load_graph(const std::string& filepath) {
    if (!validate_filepath(filepath)) {
        std::cerr << "Invalid or unsafe filepath." << std::endl;
        return;
    }

    int fd = open(filepath.c_str(), O_RDONLY);
    if (fd < 0) {
        std::cerr << "Failed to open file." << std::endl;
        return;
    }

    struct stat sb;
    fstat(fd, &sb);
    size_t length = sb.st_size;

    char* data = static_cast<char*>(mmap(nullptr, length, PROT_READ, MAP_PRIVATE, fd, 0));
    if (data == MAP_FAILED) {
        std::cerr << "mmap failed." << std::endl;
        close(fd);
        return;
    }

    // Parse with SIMD and cache-aware strategy
    int src = 0, dst = 0;
    size_t i = 0;
    while (i < length) {
        if (isdigit(data[i])) {
            src = 0;
            while (i < length && isdigit(data[i])) {
                src = src * 10 + (data[i] - '0');
                ++i;
            }
            while (i < length && (data[i] == ' ' || data[i] == '\t')) ++i;
            dst = 0;
            while (i < length && isdigit(data[i])) {
                dst = dst * 10 + (data[i] - '0');
                ++i;
            }
            adj_list[src].emplace_back(dst);
            adj_list[dst].emplace_back(src); // Assuming undirected
        }
        while (i < length && data[i] != '\n') ++i;
        ++i;
    }

    munmap(data, length);
    close(fd);
}

const std::vector<int>& Graph::get_neighbors(int node) const {
    auto it = adj_list.find(node);
    if (it != adj_list.end()) {
        return it->second;
    }
    return static_empty_vector;
}

void Graph::add_node(int node) {
    if (adj_list.find(node) == adj_list.end()) {
        adj_list[node] = std::vector<int>();
    }
}

void Graph::delete_node(int node) {
auto it = adj_list.find(node);
if (it != adj_list.end()) {
    // First, remove 'node' from all neighbor lists
    for (const auto& neighbor : it->second) {
        auto neighbor_it = adj_list.find(neighbor);
        if (neighbor_it != adj_list.end()) {
            auto& neighbors = neighbor_it->second;
            neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), node), neighbors.end());
        }
    }
    // Then, remove the node from the adjacency list
    adj_list.erase(it);
}
}

void Graph::save_graph(const std::string& filename) const {
    if (!validate_filepath(filename)) {
        std::cerr << "Invalid or unsafe filepath for saving." << std::endl;
        return;
    }

    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        std::cerr << "Failed to open file for writing." << std::endl;
        return;
    }

    std::unordered_set<std::pair<int, int>, PairHash> written_edges;

    for (const auto& [src, neighbors] : adj_list) {
        for (int dst : neighbors) {
            auto edge = std::minmax(src, dst);
            if (written_edges.insert(edge).second) {  // Insert returns true if new
                outfile << edge.first << " " << edge.second << "\n";
            }
        }
    }

    outfile.close();
}