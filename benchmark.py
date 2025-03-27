import sys
sys.path.append("./build")

import timeit
import networkx as nx

# Our graph_cpp library
setup_aegisgraph = '''
import aegisgraph
graph = aegisgraph.Graph()
graph.load_edges("./data/facebook_combined.txt")
'''
time_aegisgraph = timeit.timeit("graph.get_neighbors(42)", setup=setup_aegisgraph, number=100000)

# NetworkX
setup_networkx = '''
import networkx as nx
G = nx.read_edgelist("./data/facebook_combined.txt")
'''
time_nx = timeit.timeit('list(G.neighbors("42"))', setup=setup_networkx, number=100000)

print(f"aegisgraph: {time_aegisgraph:.4f}s | NetworkX: {time_nx:.4f}s")