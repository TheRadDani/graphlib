#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

/**
 * @file Graph.hpp
 * @brief Header for a high-performance graph class using adjacency lists.
 * @defgroup GraphLib Graph Library
 * @version 1.0
 * @date 2025-03-27
 */

namespace graph {

/**
 * @class Graph
 * @brief A memory-safe and efficient undirected graph using adjacency list representation.
 */
class alignas(64) Graph {
private:
    std::unordered_map<int, std::vector<int>> adjacency_list alignas(64);
    std::unordered_set<int> nodes alignas(64);
    static const std::vector<int> empty_vec;

    inline void validate_node_id(int id) const;

public:
    Graph() noexcept;
    ~Graph() noexcept;

    void add_edge(int src, int dest);
    bool has_node(int node) const noexcept;
    const std::vector<int>& get_neighbors(int node) const noexcept;
    void load_edges(const std::string& filename);
};

} // namespace graph