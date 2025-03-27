import sys
sys.path.append("./build")
import aegisgraph
graph = aegisgraph.Graph()
graph.load_edges("/home/daniel/graphlib/data/facebook_combined.txt")  # You need to create this file for testing
neighbors = graph.get_neighbors(1)
neighbors = graph.get_neighbors(1)
print(neighbors)
graph.delete_node(1)
graph.delete_node(1)
print(neighbors)
print(aegisgraph.__file__)