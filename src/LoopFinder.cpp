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

std::set<std::string> LoopFinder::buildLoop(const std::string& u, const std::string& v) const {
    std::set<std::string> loop;
    std::stack<std::string> st;

    loop.insert(u);
    loop.insert(v);
    st.push(u);

    while (!st.empty()) {
        std::string cur = st.top();
        st.pop();

        auto it = rev_graph.find(cur);
        if (it == rev_graph.end()) {
            continue;
        }

        for (const auto& pred : it->second) {
            if (!loop.count(pred)) {
                loop.insert(pred);
                st.push(pred);
            }
        }
    }

    return loop;
}

void LoopFinder::printLoops() const {
    std::cout << "\nloops from back edges:\n";

    for (const auto& [u, v] : backEdges) {
        std::set<std::string> loop = buildLoop(u, v);

        std::cout << "back edge " << u << " -> " << v << ": { ";
        for (const auto& node : loop) {
            std::cout << node << " ";
        }
        std::cout << "}\n";
    }
}

std::map<std::string, std::set<std::string>> LoopFinder::collectLoopsByHeader() const {
    std::map<std::string, std::set<std::string>> mergedLoops;

    for (const auto& [u, v] : backEdges) {
        std::set<std::string> loop = buildLoop(u, v);
        mergedLoops[v].insert(loop.begin(), loop.end());
    }

    return mergedLoops;
}

void LoopFinder::printMergedLoops() const {
    std::cout << "\nmerged loops by header:\n";

    std::map<std::string, std::set<std::string>> mergedLoops = collectLoopsByHeader();

    for (const auto& [header, loopNodes] : mergedLoops) {
        std::cout << "header " << header << ": { ";
        for (const auto& node : loopNodes) {
            std::cout << node << " ";
        }
        std::cout << "}\n";
    }
}

std::vector<LoopInfo> LoopFinder::buildLoopInfos() const {
    std::vector<LoopInfo> result;
    std::map<std::string, std::set<std::string>> mergedLoops = collectLoopsByHeader();

    int nextId = 1;
    for (const auto& [header, loopNodes] : mergedLoops) {
        LoopInfo loop;
        loop.id = nextId++;
        loop.header = header;
        loop.nodes = loopNodes;
        result.push_back(loop);
    }

    return result;
}

void LoopFinder::printLoopInfos() const {
    std::cout << "\nloop infos:\n";

    std::vector<LoopInfo> loops = buildLoopInfos();

    for (const auto& loop : loops) {
        std::cout << "[" << loop.id << "] header: " << loop.header << ", nodes: { ";
        for (const auto& node : loop.nodes) {
            std::cout << node << " ";
        }
        std::cout << "}\n";
    }
}

std::vector<LoopInfo> LoopFinder::buildAllLoops() const {
    std::vector<LoopInfo> loops = buildLoopInfos();

    std::set<std::string> nodesInLoops;
    for (const auto& loop : loops) {
        nodesInLoops.insert(loop.nodes.begin(), loop.nodes.end());
    }

    std::set<std::string> rootNodes;
    for (const auto& node : nodes) {
        if (!nodesInLoops.count(node)) {
            rootNodes.insert(node);
        }
    }

    std::vector<LoopInfo> result;

    LoopInfo rootLoop;
    rootLoop.id = 0;
    rootLoop.header = "";
    rootLoop.nodes = rootNodes;
    rootLoop.isRoot = true;
    result.push_back(rootLoop);

    for (auto& loop : loops) {
        loop.isRoot = false;
        result.push_back(loop);
    }

    return result;
}

void LoopFinder::printAllLoops() const {
    std::cout << "\nAll loops:\n";

    std::vector<LoopInfo> loops = buildAllLoops();

    for (const auto& loop : loops) {
        std::cout << "[" << loop.id << "] -> { ";
        for (const auto& node : loop.nodes) {
            std::cout << node << " ";
        }
        std::cout << "}";

        if (!loop.isRoot) {
            std::cout << ", header: " << loop.header;
        }

        std::cout << "\n";
    }
}