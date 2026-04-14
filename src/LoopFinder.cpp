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

void LoopFinder::dfs(const std::string& u) {
    visited.insert(u);
    inStack.insert(u);

    for (const auto& v : graph[u]) {
        if (!visited.count(v)) {
            dfs(v);
        } else if (inStack.count(v)) {
            backEdges.push_back({u, v});
        }
    }

    inStack.erase(u);
}

void LoopFinder::findBackEdges() {
    visited.clear();
    inStack.clear();
    backEdges.clear();

    for (const auto& node : nodes) {
        if (!visited.count(node)) {
            dfs(node);
        }
    }
}

void LoopFinder::printBackEdges() const {
    std::cout << "\nBack edges:\n";
    for (const auto& [u, v] : backEdges) {
        std::cout << u << " -> " << v << "\n";
    }
}