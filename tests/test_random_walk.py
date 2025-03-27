import sys
sys.path.append("./build")

import pytest
import graphlib_cpp as graphlib

def test_random_walk():
    graph = graphlib.Graph()
    graph.load_edges("/home/daniel/graphlib/data/facebook_combined.txt")  # Create a small graph for testing

    walks = graph.random_walk(start_node=1, walk_length=5, num_walks=3)
    
    assert isinstance(walks, list)
    assert len(walks) == 3
    for walk in walks:
        assert isinstance(walk, list)
        assert len(walk) == 5
