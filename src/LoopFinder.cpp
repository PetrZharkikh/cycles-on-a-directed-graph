#include "LoopFinder.h"

#include <iostream>
#include <algorithm>
using namespace std;

void LoopFinder::addEdge(const string& u, const string& v) {
    graph[u].push_back(v);
    rev_graph[v].push_back(u);
    nodes.insert(u);
    nodes.insert(v);
}

bool LoopFinder::isSubset(const set<string>& a, const set<string>& b) const {
    for (const auto& x : a) {
        if (!b.count(x)) {
            return false;
        }
    }
    return true;
}

vector<string> LoopFinder::sortNodesForPrint(const set<string>& s) const {
    // для вывода: сначала start, потом обычные, в конце stop
    vector<string> v(s.begin(), s.end());
    sort(v.begin(), v.end(), [](const string& a, const string& b) {
        auto rank = [](const string& x) {
            if (x == "start") return 0;
            if (x == "stop") return 2;
            return 1;
        };
        if (rank(a) != rank(b)) {
            return rank(a) < rank(b);
        }
        return a < b;
    });
    return v;
}

void LoopFinder::dfsNumbering(
    const string& node,
    AnalyzeState& st,
    int& timer,
    set<string>& used) const {
    used.insert(node);
    st.number[node] = timer;
    st.nodeByNum[timer] = node;
    timer++;

    auto it = graph.find(node);
    if (it != graph.end()) {
        for (const auto& to : it->second) {
            if (!used.count(to)) {
                dfsNumbering(to, st, timer, used);
            }
        }
    }

    st.last[st.number[node]] = timer - 1;
}

bool LoopFinder::isAncestor(int w, int v, const AnalyzeState& st) const {
    return (w <= v) && (v <= st.last[w]);
}

int LoopFinder::findSet(int x, vector<int>& parent) const {
    if (parent[x] == x) {
        return x;
    }
    parent[x] = findSet(parent[x], parent);
    return parent[x];
}

void LoopFinder::unionSet(int x, int y, vector<int>& parent) const {
    int rx = findSet(x, parent);
    int ry = findSet(y, parent);
    parent[rx] = ry;
}

LoopFinder::AnalyzeState LoopFinder::runHavlakAnalyze() const {
    AnalyzeState st;
    st.N = static_cast<int>(nodes.size());
    st.nodeByNum.assign(st.N + 1, "");
    st.last.assign(st.N + 1, 0);
    st.backPreds.assign(st.N + 1, {});
    st.nonBackPreds.assign(st.N + 1, {});
    st.header.assign(st.N + 1, 1);     // как в статье, по умолчанию START
    st.type.assign(st.N + 1, 0);       // nonheader

    // a) dfs numbering из start, потом добираем хвосты если вдруг есть
    int timer = 1;
    set<string> used;
    if (nodes.count("start")) {
        dfsNumbering("start", st, timer, used);
    }
    for (const auto& node : nodes) {
        if (!used.count(node)) {
            dfsNumbering(node, st, timer, used);
        }
    }

    st.N = timer - 1;
    st.nodeByNum.resize(st.N + 1);
    st.last.resize(st.N + 1);
    st.backPreds.resize(st.N + 1);
    st.nonBackPreds.resize(st.N + 1);
    st.header.resize(st.N + 1, 1);
    st.type.resize(st.N + 1, 0);
    st.ufParent.resize(st.N + 1);
    for (int i = 1; i <= st.N; i++) {
        st.ufParent[i] = i;
    }

    int startNum = st.number.count("start") ? st.number["start"] : 1;
    st.header[startNum] = 0; // у start нет родителя

    // b) делим входящие на backPred/nonBackPred
    for (const auto& [u, out] : graph) {
        if (!st.number.count(u)) {
            continue;
        }
        int vNum = st.number[u];
        for (const auto& w : out) {
            if (!st.number.count(w)) {
                continue;
            }
            int wNum = st.number[w];
            if (isAncestor(wNum, vNum, st)) {
                st.backPreds[wNum].insert(vNum);
            } else {
                st.nonBackPreds[wNum].insert(vNum);
            }
        }
    }

    // c,d,e) основной цикл analyze loops
    for (int w = st.N; w >= 1; w--) {
        set<int> P;
        for (int v : st.backPreds[w]) {
            if (v != w) {
                P.insert(findSet(v, st.ufParent));
            } else {
                st.type[w] = 1; // self
            }
        }

        vector<int> worklist(P.begin(), P.end());
        if (!P.empty() && st.type[w] == 0) {
            st.type[w] = 2; // reducible
        }

        while (!worklist.empty()) {
            int x = worklist.back();
            worklist.pop_back();

            for (int y : st.nonBackPreds[x]) {
                int yRep = findSet(y, st.ufParent);
                if (!isAncestor(w, yRep, st)) {
                    st.type[w] = 3; // irreducible
                    st.nonBackPreds[w].insert(yRep);
                } else if (yRep != w && !P.count(yRep)) {
                    P.insert(yRep);
                    worklist.push_back(yRep);
                }
            }
        }

        for (int x : P) {
            st.header[x] = w;
            unionSet(x, w, st.ufParent);
        }
    }

    return st;
}

vector<LoopInfo> LoopFinder::buildAllLoops() const {
    vector<LoopInfo> loops;
    AnalyzeState st = runHavlakAnalyze();
    if (st.N == 0) {
        return loops;
    }

    int startNum = st.number.count("start") ? st.number["start"] : 1;

    // строим дерево header для всех нод
    vector<vector<int>> children(st.N + 1);
    for (int v = 1; v <= st.N; v++) {
        int p = st.header[v];
        if (p >= 1 && p <= st.N) {
            children[p].push_back(v);
        }
    }

    // loop headers = self/reducible/irreducible
    set<int> loopHeaders;
    for (int v = 1; v <= st.N; v++) {
        if (st.type[v] == 1 || st.type[v] == 2 || st.type[v] == 3) {
            loopHeaders.insert(v);
        }
    }

    map<int, set<string>> loopNodesByHeader;
    for (int h : loopHeaders) {
        stack<int> ss;
        ss.push(h);
        while (!ss.empty()) {
            int cur = ss.top();
            ss.pop();
            loopNodesByHeader[h].insert(st.nodeByNum[cur]);
            for (int to : children[cur]) {
                ss.push(to);
            }
        }
    }

    // сортируем headers в удобном порядке для стабильных id
    vector<int> orderedHeaders(loopHeaders.begin(), loopHeaders.end());
    sort(orderedHeaders.begin(), orderedHeaders.end(), [&](int a, int b) {
        const string& sa = st.nodeByNum[a];
        const string& sb = st.nodeByNum[b];
        auto rank = [](const string& x) {
            if (x == "start") return 0;
            if (x == "stop") return 2;
            return 1;
        };
        if (rank(sa) != rank(sb)) {
            return rank(sa) < rank(sb);
        }
        if (sa != sb) {
            return sa < sb;
        }
        return a < b;
    });

    int nextId = 1;
    for (int h : orderedHeaders) {
        if (h == startNum) {
            // start это корень, отдельным loop id не считаем
            continue;
        }

        LoopInfo info;
        info.id = nextId++;
        info.header = st.nodeByNum[h];
        info.nodes = loopNodesByHeader[h];

        if (st.type[h] == 3) {
            info.type = "irreducible";
        } else {
            info.type = "reducible";
        }

        loops.push_back(info);
    }

    return loops;
}

vector<pair<int, int>> LoopFinder::buildLoopTree(const vector<LoopInfo>& loops) const {
    vector<pair<int, int>> tree;

    for (const auto& child : loops) {
        int parentId = 0;
        int bestSize = -1;

        for (const auto& parent : loops) {
            if (parent.id == child.id) {
                continue;
            }
            if (!isSubset(child.nodes, parent.nodes)) {
                continue;
            }

            int parentSize = static_cast<int>(parent.nodes.size());
            if (bestSize == -1 || parentSize < bestSize) {
                bestSize = parentSize;
                parentId = parent.id;
            }
        }

        tree.push_back({parentId, child.id});
    }

    return tree;
}

map<int, set<string>> LoopFinder::buildLoopBlocks(const vector<LoopInfo>& loops) const {
    // каждую вершину кладем в самый внутренний цикл
    map<int, set<string>> blocks;

    for (const auto& node : nodes) {
        int owner = 0;
        int bestSize = -1;

        for (const auto& loop : loops) {
            if (!loop.nodes.count(node)) {
                continue;
            }
            int curSize = static_cast<int>(loop.nodes.size());
            if (bestSize == -1 || curSize < bestSize) {
                bestSize = curSize;
                owner = loop.id;
            }
        }

        blocks[owner].insert(node);
    }

    return blocks;
}

void LoopFinder::printFinalResult() const {
    vector<LoopInfo> loops = buildAllLoops();
    vector<pair<int, int>> tree = buildLoopTree(loops);
    map<int, set<string>> blocks = buildLoopBlocks(loops);

    cout << "\nFinal result:\n";
    cout << "Loop blocks:\n";

    cout << "[0] -> { ";
    for (const auto& node : sortNodesForPrint(blocks[0])) {
        cout << node << " ";
    }
    cout << "}\n";

    for (const auto& loop : loops) {
        cout << "[" << loop.id << "] -> { ";
        for (const auto& node : sortNodesForPrint(blocks[loop.id])) {
            cout << node << " ";
        }
        cout << "}, header: " << loop.header << ", " << loop.type << "\n";
    }

    cout << "Loop tree:\n";
    for (const auto& [parent, child] : tree) {
        cout << "{" << parent << ", " << child << "}\n";
    }
}