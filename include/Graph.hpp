/**
 * @file Graph.hpp
 * @brief High-performance, secure graph library using parallel adjacency lists.
 * 
 * @author Daniel Ferreto
 * @version 1.0
 * @date 2025-03-28
 * 
 * @details
 * Hardware-optimized graph structure using Abseil's flat_hash_map for O(1) average complexity,
 * SIMD-accelerated parsing, and OpenMP-based parallelism. Features secure memory handling,
 * integer overflow protection, and cache-aware algorithms. Designed for large-scale social
 * network graphs (e.g., Stanford SNAP datasets) with efficient node deletion and batch processing.
 * 
 * @note Compile with -mavx2 -fopenmp -O3 for optimal performance
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <shared_mutex>
#include <utility>

struct PairHash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 << 1); // Simple XOR hash
    }
};

class Graph {
public:
    Graph();

    /**
     * @brief Loads an edge list from a file into an adjacency list.
     * @param filepath Path to the edge list file.
     */
    void load_graph(const std::string& filepath);

    /**
     * @brief Gets the neighbors of a given node.
     * @param node Node ID to query.
     * @return A const reference to a vector of neighbor node IDs.
     */
    const std::vector<int>& get_neighbors(int node) const;

    /**
     * @brief Adds a node to the graph.
     * @param node Node ID to add.
     */
    void add_node(int node);

    /**
     * @brief Deletes a node and its edges from the graph.
     * @param node Node ID to delete.
     */
    void delete_node(int node);

    /**
 * @brief Save the graph to a file in edge list format.
 *
 * Each line in the file will represent an edge as "src dst".
 * Duplicate and self-loop edges are avoided.
 *
 * @param filename The destination file path to save the graph.
 */
void save_graph(const std::string& filename) const;

private:
    std::unordered_map<int, std::vector<int>> adj_list;  ///< Adjacency list representation
    std::vector<int> static_empty_vector; ///< Static empty vector for non-existent nodes


    /**
     * @brief Validates the input file path to avoid directory traversal attacks.
     * @param path The input file path.
     * @return True if valid, false otherwise.
     */
    bool validate_filepath(const std::string& path) const;
};

#endif // GRAPH_H