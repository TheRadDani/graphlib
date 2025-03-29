import timeit
import networkx as nx
import aegisgraph
import random

# Our graph_cpp library
setup_aegisgraph = '''
import aegisgraph
graph = aegisgraph.Graph()
graph.load_graph("./data/facebook_combined.txt")
'''
time_aegisgraph = timeit.timeit("graph.get_neighbors(42)", setup=setup_aegisgraph, number=1000000)

# NetworkX
setup_networkx = '''
import networkx as nx
G = nx.read_edgelist("./data/facebook_combined.txt")
'''
time_nx = timeit.timeit('G.neighbors("42")', setup=setup_networkx, number=1000000)

print(f"aegisgraph: {time_aegisgraph:.4f}s | NetworkX: {time_nx:.4f}s")


import aegisgraph
graph = aegisgraph.Graph()
graph.load_graph("./data/facebook_combined.txt")
neighbor = graph.get_neighbors(42)
print("Neighbors of node 42 with aegisgraph:", neighbor)

import networkx as nx
G = nx.read_edgelist("./data/facebook_combined.txt")
neighbors = list(G.neighbors("42"))
print("Neighbors of node 42 with nx:", neighbors)


import networkx as nx
import random

# Load the graph
G = nx.read_edgelist("./data/facebook_combined.txt")

def random_walk(graph, start_node, walk_length):
    walk = [start_node]
    current_node = start_node
    
    for _ in range(walk_length - 1):
        neighbors = list(graph.neighbors(current_node))
        if not neighbors:  # If no neighbors, stop the walk
            break
        next_node = random.choice(neighbors)
        walk.append(next_node)
        current_node = next_node
    
    return walk

# Parameters
start_node = '10'  # Replace with your desired starting node
walk_length = 5  # Length of each walk
num_walks = 3     # Number of walks

# Generate multiple random walks
random_walks = []
for _ in range(num_walks):
    walk = random_walk(G, start_node, walk_length)
    random_walks.append(walk)

# Output the walks
for i, walk in enumerate(random_walks):
    print(f"Walk {i+1}: {walk}")


# NetworkX
# Define the setup code
setup_networkx = '''
import networkx as nx
import random

# Load the graph
G = nx.read_edgelist("./data/facebook_combined.txt")

def random_walk(graph, start_node, walk_length):
    walk = [start_node]
    current_node = start_node
    
    for _ in range(walk_length - 1):
        neighbors = list(graph.neighbors(current_node))
        if not neighbors:  # If no neighbors, stop the walk
            break
        next_node = random.choice(neighbors)
        walk.append(next_node)
        current_node = next_node
    
    return walk
'''

# Code to be timed
stmt = '''
# Parameters
start_node = '10'  # Replace with your desired starting node
walk_length = 5  # Length of each walk
num_walks = 3     # Number of walks

# Generate multiple random walks
random_walks = []
for _ in range(num_walks):
    walk = random_walk(G, start_node, walk_length)
    random_walks.append(walk)
'''

# Time the function execution
time_nx = timeit.timeit(stmt, setup=setup_networkx, number=1000000)


setup_aegisgraph ="""
import sys
sys.path.append("./build")
import aegisgraph
graph = aegisgraph.Graph()
graph.load_graph("/home/daniel/aegisgraph/data/facebook_combined.txt")  # You need to create this file for testing
"""

time_aegisgraph = timeit.timeit("graph.random_walk(start_node=10, walk_length=5, num_walks=3)", setup=setup_aegisgraph, number=1000000)


print(f"aegisgraph: {time_aegisgraph:.4f}s | NetworkX: {time_nx:.4f}s")