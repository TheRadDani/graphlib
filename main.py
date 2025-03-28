import sys
sys.path.append("./build")
import aegisgraph
graph = aegisgraph.Graph()
graph.load_graph("/home/daniel/aegisgraph/data/facebook_combined.txt")  # You need to create this file for testing
neighbors = graph.get_neighbors(1)
neighbors = graph.get_neighbors(1)
print(neighbors)
graph.delete_node(1)
neighbors = graph.delete_node(1)
print(neighbors)
print(aegisgraph.__file__)

walks = graph.random_walk(start_node=10, walk_length=5, num_walks=3)
print(walks)

graph.save_graph("test.txt")