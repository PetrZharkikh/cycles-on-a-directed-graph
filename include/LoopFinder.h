#pragma once

#include <unordered_map>
#include <vector>
#include <set>
#include <string>
#include <utility>
#include <stack>
#include <map>

class LoopFinder {
public:
    std::unordered_map<std::string, std::vector<std::string>> graph;
    std::unordered_map<std::string, std::vector<std::string>> rev_graph;
    std::set<std::string> nodes;

    void addEdge(const std::string& u, const std::string& v);
    void printGraph();

    std::set<std::string> visited;  //не заходим в вершину воторой раз
    std::set<std::string> inStack;  //вершины на пути рекурсии
    std::vector<std::pair<std::string, std::string>> backEdges; //список обратных ребер

    void dfs(const std::string& u);
    void findBackEdges();
    void printBackEdges() const;

    std::set<std::string> buildLoop(const std::string& u, const std::string& v) const;
    void printLoops() const;

    std::map<std::string, std::set<std::string>> collectLoopsByHeader() const;
    void printMergedLoops() const;
};