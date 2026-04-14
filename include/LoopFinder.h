#pragma once

#include <unordered_map>
#include <vector>
#include <set>
#include <string>

class LoopFinder {
public:
    std::unordered_map<std::string, std::vector<std::string>> graph;
    std::unordered_map<std::string, std::vector<std::string>> rev_graph;
    std::set<std::string> nodes;

    void addEdge(const std::string& u, const std::string& v);
    void printGraph();
};