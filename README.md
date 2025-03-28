# 🚀 AegisGraph: High-Performance Graph Library in plain C++ for Python

<p align="center">
  <img src="images/aegrisgraph_logo.gif" alt="AegrisGraph Logo" style="display: block; margin: 0 auto; width: 750px; max-width: 100%;">
</p>

## 📚 Introduction
**AegisGraph** is a high-performance graph library written in **C++** with Python bindings. It is designed to provide a fast and efficient alternative to popular graph libraries such as:

- **NetworkX** – Graph analysis library in Python.
- **PyG (PyTorch Geometric)** – Deep learning framework for graph-based learning.

AegisGraph focuses on **hardware efficiency, memory optimization, and security**, offering superior speed and scalability in graph operations.

---

## 🎯 Why Choose AegisGraph?

### 🔥 Key Advantages
- **Adjacency List Representation:**  
  Optimized graph storage that enhances memory efficiency and query speed compared to sparse matrix representations.
- **C++ Backend for Maximum Performance:**  
  Core operations are implemented in C++ with minimal overhead, ensuring faster graph processing.
- **Secure and Efficient Random Walks:**  
  Utilizes **Pseudorandom Number Generators (PRNGs)** instead of Python’s `random.choice()` for secure and reproducible random walks.
- **Optimized RAM Usage:**  
  Memory-efficient graph loading and neighbor queries for handling large-scale graphs.
- **No PyTorch Dependency:**  
  Lightweight deployment without heavy dependencies, making it ideal for various environments.

---

## ⚡ Core Features
- 📡 **Graph Construction:** Supports directed, undirected, and weighted graphs.
- 🔄 **Random Walks with PRNG:** Secure and hardware-efficient random walk implementation.
- 🕸️ **Graph Algorithms:** Supports BFS, DFS, Dijkstra’s, and other common algorithms.
- 🧠 **Fast Neighbor Queries:** Constant-time neighbor access through adjacency lists.
- 📊 **Graph Serialization:** Save and load graphs efficiently.

---

## 🛠️ Installation

### 📦 Prerequisites
- **C++ Compiler:** GCC, Clang, or MSVC supporting C++17 or later.
- **Python (Optional for Bindings):** Version 3.10 or higher.
- **CMake:** For building the library.

### ⚙️ Building from Source

#### 1. Clone the Repository
```bash
git clone https://github.com/your-username/aegisgraph.git
cd aegisgraph
mkdir build
cd build
cmake ..
make
```

#### 2. Install the Python Bindings

🐍 Python Usage
```python
import aegisgraph

# Create a graph instance
graph = aegisgraph.Graph()

# Add edges
graph.add_edge(0, 1)
graph.add_edge(1, 2)
graph.add_edge(2, 3)
graph.add_edge(3, 0)

graph.getNeighbor(1)

# Perform a random walk
path = graph.random_walk(0, 10)
print(f"Random walk path: {path}")
```

---

#### Install with pip
```bash
python3 -m venv .aegisgraph-env
source .aegisgraph-env/bin/activate
pip install --upgrade pip build
pip install -e .
```


🚀 Graph Loading Speed
AegisGraph achieves a 3x speed improvement in loading large-scale graphs compared to NetworkX.

---

📄 API Documentation
🚀 Graph Class
* addEdge(int u, int v, double weight = 1.0) – Add an edge between nodes.

* getNeighbors(int node) – Retrieve neighbors of a node.

* randomWalk(int start, int steps) – Perform a random walk starting from a node.

🔍 Utility Functions
* loadGraph(string filename) – Load graph from a file.

* saveGraph(string filename) – Save graph to a file.

---

🐞 Reporting Issues
To report a bug or request a feature, please [open an issue](https://github.com/TheRadDani/aegisgraph/issues)
.

---

📫 Contact
For questions, suggestions, or contributions, reach out via:

📧 Email: ldanielfch@gmail.com

🌐 GitHub: [TheRadDani](https://github.com/TheRadDani/)

📚 [View Full API Documentation (HTML)](https://theraddani.github.io/aegisgraph/)

## 📖 Citation

If you use AegisGraph in academic work, please cite it as:

```bibtex
@misc{aegisgraph_2025,
  author       = {Daniel F. C.}, 
  title        = {AegisGraph: High-Performance Graph Library in plain C++ for Python},
  year         = {2025},
  howpublished = {\url{https://github.com/TheRadDani/aegisgraph}},
  note         = {Software library for efficient graph processing}
}