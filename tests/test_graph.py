class Graph:
    def __init__(self):
        self.nodes = set()
        self.edges = {}

    def add_node(self, node):
        self.nodes.add(node)
        self.edges[node] = []

    def delete_node(self, node):
        if node in self.nodes:
            self.nodes.remove(node)
            self.edges.pop(node, None)

    def get_neighbors(self, node):
        if node not in self.nodes:
            raise RuntimeError(f"Node {node} does not exist.")
        return self.edges.get(node, [])
