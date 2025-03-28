/**
 * @file aegisgraph.cpp
 * @brief Pybind11 bindings for aegisgraph C++ library (Graph and RandomWalker).
 *
 * @author Daniel Ferreto
 * @version 1.1
 * @date 2025-03-27
 *
 * This file binds the C++ Graph and RandomWalker classes to Python for high-performance
 * graph processing and sampling capabilities. It uses pybind11 with best practices
 * for secure, optimized, and minimal-overhead bindings.
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "Graph.hpp"
#include "RandomWalker.hpp"

// Compiler optimization hints
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC optimize("O3")
#pragma GCC target("tune=native")
#endif

namespace py = pybind11;

PYBIND11_MODULE(aegisgraph, m) {
    m.doc() = "aegisgraph: High-performance graph processing library in C++ (secure, hardware-aware)";

    py::class_<Graph>(m, "Graph")
        .def(py::init<>(), "Create an empty graph instance.")
        .def("load_graph", &Graph::load_graph, py::arg("filepath"),
            R"pbdoc(
                Load an edge list from a file.

                Args:
                    filepath (str): Path to the edge list file.
            )pbdoc")
        .def("get_neighbors", &Graph::get_neighbors, py::arg("node"),
            R"pbdoc(
                Get the neighbors of a given node.

                Args:
                    node (int): Node ID.

                Returns:
                    List[int]: Neighbor node IDs.
            )pbdoc")
        .def("add_node", &Graph::add_node, py::arg("node"),
            R"pbdoc(Add a node to the graph.)pbdoc")
        .def("delete_node", &Graph::delete_node, py::arg("node"),
            R"pbdoc(Delete a node and its edges from the graph.)pbdoc")
        .def("save_graph", &Graph::save_graph, py::arg("filename"),
        R"pbdoc(
            Save the graph to a file in edge list format.

            Args:
                filename (str): Path where the graph will be saved.
        )pbdoc")
        .def("random_walk", [](Graph& self, int start_node, int walk_length, int num_walks) {
            // Stack-allocated, short-lived walker
            RandomWalker walker;
            return walker.random_walk(self, start_node, walk_length, num_walks);
        }, py::arg("start_node"), py::arg("walk_length"), py::arg("num_walks"),
        R"pbdoc(
            Generate random walks starting from a node.

            Args:
                start_node (int): Starting node.
                walk_length (int): Number of steps.
                num_walks (int): Number of walks.

            Returns:
                List[List[int]]: Random walks.
        )pbdoc");

    py::class_<RandomWalker>(m, "_RandomWalker") // Internal for advanced users
        .def(py::init<>())
        .def("random_walk", &RandomWalker::random_walk,
            py::arg("graph"), py::arg("start_node"), py::arg("walk_length"), py::arg("num_walks"),
            R"pbdoc(
                Generate random walks from a start node.

                Args:
                    graph (Graph): The graph object.
                    start_node (int): Starting node ID.
                    walk_length (int): Number of steps in each walk.
                    num_walks (int): Number of walks.

                Returns:
                    List[List[int]]: A list of random walks.
            )pbdoc");
}