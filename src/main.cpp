#include "LoopFinder.h"

int main() {
    LoopFinder lf;

    lf.addEdge("start", "1");
    lf.addEdge("1", "2");
    lf.addEdge("1", "3");
    lf.addEdge("2", "3");
    lf.addEdge("3", "4");
    lf.addEdge("4", "3");
    lf.addEdge("4", "5");
    lf.addEdge("5", "2");
    lf.addEdge("5", "stop");

    lf.findBackEdges();
    lf.printFinalResult();

    return 0;
}