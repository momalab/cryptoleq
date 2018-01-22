#include <iostream>
#include "cuddObj.hh"

int main()
{
    Cudd mgr(0, 0);
    BDD x0 = mgr.bddVar();
    BDD x1 = mgr.bddVar();
    BDD x2 = mgr.bddVar();
    BDD x3 = mgr.bddVar();
    BDD f = x0 * x1 + x2; //*x3;

    mgr.ReduceHeap();

    std::cout << f << '\n';
}