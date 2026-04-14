#include "LoopFinder.h"
#include <iostream>
#include <regex>
#include <sstream>

using namespace std;

bool readGraphFromInput(LoopFinder& lf) {
    // ввод:{start,1}, {1,2}, ...
    string all;
    string line;
    while (getline(cin, line)) {
        all += line;
        all += "\n";
    }

    regex edgePattern(R"(\{\s*([^,\s\}]+)\s*,\s*([^,\s\}]+)\s*\})");
    sregex_iterator it(all.begin(), all.end(), edgePattern);
    sregex_iterator end;

    bool hasEdges = false;
    for (; it != end; ++it) {
        string u = (*it)[1].str();
        string v = (*it)[2].str();
        lf.addEdge(u, v);
        hasEdges = true;
    }

    return hasEdges;
}

int main() {
    LoopFinder lf;

    if (!readGraphFromInput(lf)) {
        cout << "no edges found in input\n";
        cout << "example: {start,1}, {1,2}, {2,stop}\n";
        return 0;
    }

    lf.printFinalResult();

    return 0;
}