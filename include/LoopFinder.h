#pragma once

#include <unordered_map>
#include <vector>
#include <set>
#include <string>
#include <utility>
#include <map>
#include <stack>
#include <functional>

struct LoopInfo {
    int id;
    std::string header;
    std::set<std::string> nodes;
    std::string type;
};

class LoopFinder {
public:
    std::unordered_map<std::string, std::vector<std::string>> graph;
    std::unordered_map<std::string, std::vector<std::string>> rev_graph;
    std::set<std::string> nodes;

    void addEdge(const std::string& u, const std::string& v);
    void printFinalResult() const;

private:
    std::vector<LoopInfo> buildAllLoops() const;
    std::vector<std::pair<int, int>> buildLoopTree(const std::vector<LoopInfo>& loops) const;
    std::map<int, std::set<std::string>> buildLoopBlocks(const std::vector<LoopInfo>& loops) const;

    struct AnalyzeState {
        int N = 0;
        std::map<std::string, int> number;
        std::vector<std::string> nodeByNum;
        std::vector<int> last;
        std::vector<std::set<int>> backPreds;
        std::vector<std::set<int>> nonBackPreds;
        std::vector<int> header;
        std::vector<int> ufParent;
        std::vector<int> type; // 0 nonheader, 1 self, 2 reducible, 3 irreducible
    };

    AnalyzeState runHavlakAnalyze() const;
    void dfsNumbering(
        const std::string& node,
        AnalyzeState& st,
        int& timer,
        std::set<std::string>& used) const;
    bool isAncestor(int w, int v, const AnalyzeState& st) const;
    int findSet(int x, std::vector<int>& parent) const;
    void unionSet(int x, int y, std::vector<int>& parent) const;

    bool isSubset(const std::set<std::string>& a, const std::set<std::string>& b) const;
    std::vector<std::string> sortNodesForPrint(const std::set<std::string>& s) const;
};