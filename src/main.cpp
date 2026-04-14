#include "LoopFinder.h"

int main() {
    LoopFinder lf;

    lf.addEdge("start", "1");
    lf.addEdge("1", "2");
    lf.addEdge("2", "1");
    lf.addEdge("2", "3");
    lf.addEdge("3", "1");
    lf.addEdge("3", "4");
    lf.addEdge("4", "stop");

    lf.printGraph();

    lf.findBackEdges();
    lf.printBackEdges();

    lf.printLoops();
    lf.printMergedLoops();
    lf.printLoopInfos();
    lf.printAllLoops();
    lf.printLoopTree();

    return 0;
}