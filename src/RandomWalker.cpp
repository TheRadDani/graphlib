#include "RandomWalker.h"
#include <chrono>
#include <algorithm>
#include <stdexcept>

RandomWalker::RandomWalker() {
    std::random_device rd;
    pcg_engine.seed(rd());
    seed_xoshiro();
}

void RandomWalker::seed_xoshiro() {
    std::seed_seq seq{
        static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()),
        static_cast<unsigned int>(pcg_engine()),
        static_cast<unsigned int>(pcg_engine()),
        static_cast<unsigned int>(pcg_engine())
    };
    seq.generate(xoshiro_state, xoshiro_state + 4);
}

uint64_t RandomWalker::xoshiro_next() {
    uint64_t result = xoshiro_state[0] + xoshiro_state[3];

    uint64_t t = xoshiro_state[1] << 17;

    xoshiro_state[2] ^= xoshiro_state[0];
    xoshiro_state[3] ^= xoshiro_state[1];
    xoshiro_state[1] ^= xoshiro_state[2];
    xoshiro_state[0] ^= xoshiro_state[3];

    xoshiro_state[2] ^= t;

    xoshiro_state[3] = (xoshiro_state[3] << 45) | (xoshiro_state[3] >> (64 - 45));

    return result;
}

std::vector<std::vector<int>> RandomWalker::random_walk(const Graph& graph, int start_node, int walk_length, int num_walks) {
    std::vector<std::vector<int>> walks;
    walks.reserve(num_walks);

    const std::vector<int>& start_neighbors = graph.get_neighbors(start_node);
    if (start_neighbors.empty()) return walks;

    for (int i = 0; i < num_walks; ++i) {
        std::vector<int> walk;
        walk.reserve(walk_length);

        int current = start_node;
        walk.push_back(current);

        for (int step = 1; step < walk_length; ++step) {
            const std::vector<int>& neighbors = graph.get_neighbors(current);
            if (neighbors.empty()) break;

            uint64_t rnd = xoshiro_next();
            size_t index = rnd % neighbors.size();
            current = neighbors[index];
            walk.push_back(current);
        }
        walks.emplace_back(std::move(walk));
    }

    return walks;
}