import sys
sys.path.append("./build")

import pytest
import graphlib_cpp as graphlib

def test_add_and_get_neighbors():
    graph = graphlib.Graph()
    graph.add_node(1)
    graph.add_node(2)
    graph.add_node(3)

    graph.load_edges("/home/daniel/graphlib/data/facebook_combined.txt")  # You need to create this file for testing

    neighbors = graph.get_neighbors(1)
    assert isinstance(neighbors, list)
    assert all(isinstance(n, int) for n in neighbors)

def test_delete_node():
    graph = graphlib.Graph()
    graph.add_node(42)
    #graph.delete_node(42)

    with pytest.raises(RuntimeError):
        graph.get_neighbors(42)
