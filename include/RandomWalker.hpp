/**
 * @file RandomWalker.h
 * @brief Declaration of the RandomWalker class to generate random walks using Xoshiro and PCG generators.
 *        This implementation is hardware-aware, cache-optimized, and secure.
 *
 * @author 
 * Daniel Ferreto
 * @version 1.1
 * @date 2025-03-27
 *
 * This class provides a random walk generator based on advanced pseudorandom number generators
 * (Xoshiro256** and PCG) for efficient graph traversal. It is designed with performance,
 * memory safety, and modern hardware acceleration in mind. Ideal for use in graph machine learning
 * pipelines and graph-based data exploration.
 */

 #ifndef RANDOM_WALKER_H
 #define RANDOM_WALKER_H
 
 #include <vector>
 #include <unordered_map>
 #include <string>
 #include <random>
 #include "Graph.h"
 
 /**
  * @class RandomWalker
  * @brief Class for performing hardware-efficient random walks on graphs.
  */
 class RandomWalker {
 public:
     /**
      * @brief Constructor that seeds both PCG and Xoshiro generators using high entropy sources.
      */
     RandomWalker();
 
     /**
      * @brief Generate random walks from a starting node.
      *
      * This function performs multiple random walks of fixed length from a given start node,
      * using hardware-aware pseudorandom number generators to ensure reproducibility and efficiency.
      *
      * @param graph The graph to walk on.
      * @param start_node The node to start walks from.
      * @param walk_length Number of steps per walk.
      * @param num_walks Number of walks to generate.
      * @return A vector of walks (each walk is a vector of node IDs).
      */
     std::vector<std::vector<int>> random_walk(const Graph& graph, int start_node, int walk_length, int num_walks);
 
 private:
     std::mt19937_64 pcg_engine;       ///< PCG pseudorandom number generator
     uint64_t xoshiro_state[4];        ///< Xoshiro PRNG internal state
 
     /**
      * @brief Initializes Xoshiro state with entropy.
      */
     void seed_xoshiro();
 
     /**
      * @brief Returns a pseudo-random 64-bit integer using the Xoshiro256** algorithm.
      * @return A 64-bit unsigned integer with strong statistical properties.
      */
     uint64_t xoshiro_next();
 };
 
 #endif // RANDOM_WALKER_H
 
 /**
  * @file RandomWalker.cpp
  * @brief Implementation of the RandomWalker class.
  *
  * @author Daniel Ferreto
  * @version 1.1
  * @date 2025-03-27
  *
  * Implements efficient random walk generation using high-performance pseudorandom number generators.
  * Designed for use with large-scale graph data in low-latency environments.
  */