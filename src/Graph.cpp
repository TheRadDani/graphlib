#include "Graph.hpp"
#include <fstream>
#include <stdexcept>
#include <limits>
#include <memory>
#include <cstring>
#include <immintrin.h>
#include <cstdio>
#include <algorithm>

namespace graph {

// Static empty vector
const std::vector<int> Graph::empty_vec = {};

// Constructor & Destructor
Graph::Graph() noexcept = default;
Graph::~Graph() noexcept = default;

// Input validation
inline void Graph::validate_node_id(int id) const {
    if (id < 0 || id > std::numeric_limits<int>::max()) {
        throw std::invalid_argument("Invalid node ID: must be non-negative and within integer bounds.");
    }
}

// Add edge with bounds-checking
void Graph::add_edge(int src, int dest) {
    validate_node_id(src);
    validate_node_id(dest);

    if (src == dest) return;

    adjacency_list[src].emplace_back(dest);
    adjacency_list[dest].emplace_back(src);

    if (nodes.size() % 32 == 0) {
        nodes.reserve(nodes.size() + 32);
    }

    nodes.insert(src);
    nodes.insert(dest);
}

// Check node existence
bool Graph::has_node(int node) const noexcept {
    return nodes.find(node) != nodes.end();
}

// Get neighbors
const std::vector<int>& Graph::get_neighbors(int node) const noexcept {
    const auto it = adjacency_list.find(node);
    return (it != adjacency_list.end()) ? it->second : empty_vec;
}

// Remove node and all edges pointing to it
void Graph::remove_node(int node) {
    if (!has_node(node)) return;

    const auto& neighbors = adjacency_list[node];

    for (int neighbor : neighbors) {
        auto& vec = adjacency_list[neighbor];
        vec.erase(std::remove(vec.begin(), vec.end(), node), vec.end());
    }

    adjacency_list.erase(node);
    nodes.erase(node);
}

// Load edge list from file securely
void Graph::load_edges(const std::string& filename) {
    if (filename.find("..") != std::string::npos || filename.empty()) {
        throw std::invalid_argument("Invalid file path or path traversal detected.");
    }

    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) throw std::runtime_error("Failed to open graph file.");

    std::streamsize size = file.tellg();
    if (size <= 0) throw std::runtime_error("Empty or invalid graph file.");
    file.seekg(0);

    std::vector<char> raw_buffer(static_cast<size_t>(size) + 64, 0);
    char* aligned_buf = reinterpret_cast<char*>(
        (reinterpret_cast<uintptr_t>(raw_buffer.data()) + 63) & ~uintptr_t(63)
    );

    file.read(aligned_buf, size);
    file.close();

    const char* ptr = aligned_buf;
    const char* end = aligned_buf + size;

    while (ptr < end) {
        int src = 0, dest = 0;
        int read = std::sscanf(ptr, "%d %d", &src, &dest);
        if (read == 2) {
            try {
                add_edge(src, dest);
            } catch (const std::invalid_argument&) {
                // Skip malformed edge
            }
        }

        while (ptr < end && *ptr != '\n') ++ptr;
        if (*ptr == '\n') ++ptr;
    }

    #ifdef __AVX2__
    // TODO: Implement AVX2 vectorized edge parser
    #endif
}

} // namespace graph