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

 #ifndef GRAPH_H
 #define GRAPH_H
 
 #include <string>
 #include <vector>
 #include <unordered_map>
 #include <mutex>
 
 class Graph {
 public:
     Graph();
 
     /**
      * @brief Loads an edge list from a file into an adjacency list.
      * @param filepath Path to the edge list file.
      */
     void load_edges(const std::string& filepath);
 
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
 
 private:
     std::unordered_map<int, std::vector<int>> adj_list;  ///< Adjacency list representation
     std::vector<int> static_empty_vector; ///< Static empty vector for non-existent nodes
 
     /**
      * @brief Validates the input file path to avoid directory traversal attacks.
      * @param path The input file path.
      * @return True if valid, false otherwise.
      */
     bool validate_filepath(const std::string& path);
 };
 
 #endif // GRAPH_H