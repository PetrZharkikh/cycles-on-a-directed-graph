#include "LoopFinder.h"
#include <iostream>

using namespace std;

void LoopFinder::addEdge(const string& u, const string& v) {
    graph[u].push_back(v);
    rev_graph[v].push_back(u);
    nodes.insert(u);
    nodes.insert(v);
}

void LoopFinder::printGraph() {
    cout << "Graph:\n";
    for (const auto& [u, neighbors] : graph) {
        cout << u << " -> ";
        for (const auto& v : neighbors) {
            cout << v << " ";
        }
        cout << "\n";
    }
}