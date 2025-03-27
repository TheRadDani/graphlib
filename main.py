import graphlib

# Load a graph from an edge list files
g = graphlib.Graph()
g.load_edges("facebook_combined.txt")

# Check neighbors of node 42
neighbors = g.get_neighbors(42) # Returns a list in microseconds

# Generate 10 ranodm walks of length 5 starting at node 42
walks = g.random_walk(42, 5, 10)