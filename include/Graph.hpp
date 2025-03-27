#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

/**
 * @file Graph.hpp
 * @brief High-performance, secure graph library using adjacency lists.
 * 
 * @author
 * Daniel Ferreto
 * 
 * @version 1.1
 * @date 2025-03-27
 * 
 * @details
 * This header defines a memory-efficient, hardware-aware undirected graph structure,
 * designed to scale to large real-world datasets such as those from Stanford SNAP (e.g., Facebook Graph).
 * It supports safe edge insertion, neighbor queries, file loading, and node removal.
 */

namespace graph {

/**
 * @class Graph
 * @brief A secure, memory-optimized undirected graph using adjacency lists.
 * 
 * The graph supports:
 * - Efficient insertion and lookup (O(1) on average)
 * - Safe removal of nodes and their associated edges
 * - Secure edge loading from files (e.g., SNAP dataset format)
 * - Bounds-checked input validation
 */
class alignas(64) Graph {
private:
    /**
     * @brief Maps node IDs to a vector of neighbor node IDs.
     * Optimized with cache-line alignment for better locality.
     */
    std::unordered_map<int, std::vector<int>> adjacency_list alignas(64);

    /**
     * @brief Registry of existing node IDs.
     * Enables O(1) node existence checks.
     */
    std::unordered_set<int> nodes alignas(64);

    /**
     * @brief Reference to an empty vector, returned for non-existent nodes.
     */
    static const std::vector<int> empty_vec;

    /**
     * @brief Validates that a node ID is within allowed bounds.
     * @param id The node ID to check.
     * @throws std::invalid_argument If the node is negative or out-of-bounds.
     */
    inline void validate_node_id(int id) const;

public:
    /**
     * @brief Constructs an empty graph.
     */
    Graph() noexcept;

    /**
     * @brief Destructor.
     */
    ~Graph() noexcept;

    /**
     * @brief Adds an undirected edge between two nodes.
     * If nodes are not yet present, they will be created.
     * 
     * @param src Source node ID
     * @param dest Destination node ID
     * @throws std::invalid_argument If node IDs are invalid
     */
    void add_edge(int src, int dest);

    /**
     * @brief Removes a node and all edges connected to it.
     * 
     * @param node Node ID to remove
     * @note If the node does not exist, the operation is ignored.
     */
    void remove_node(int node);

    /**
     * @brief Checks whether a node exists in the graph.
     * 
     * @param node Node ID to check
     * @return true if the node is present, false otherwise
     */
    bool has_node(int node) const noexcept;

    /**
     * @brief Retrieves the neighbors of a given node.
     * 
     * @param node Node ID
     * @return A constant reference to a vector of neighbors.
     * @note Returns a static empty vector if node does not exist.
     */
    const std::vector<int>& get_neighbors(int node) const noexcept;

    /**
     * @brief Loads an edge list from a file and populates the graph.
     * 
     * Each line must be a pair of space-separated integers representing an undirected edge.
     * 
     * @param filename Path to the file (e.g., facebook_combined.txt)
     * @throws std::invalid_argument For unsafe or invalid paths
     * @throws std::runtime_error If the file cannot be read
     */
    void load_edges(const std::string& filename);
};

} // namespace graph