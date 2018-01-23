#include <iostream>
#include "cuddObj.hh"

int main()
{
    Cudd mgr(0, 0);
    BDD x0 = mgr.bddVar();
    BDD x1 = mgr.bddVar();
    BDD x2 = mgr.bddVar();
    BDD x3 = mgr.bddVar();
    BDD f = x0 ^ x1 ;//+ x2; //*x3;
    //BDD f2 = x0 + x1 * x2; //*x3;

    mgr.ReduceHeap();

    //std::cout <<"\no Blif:\n";	
    //mgr.DumpBlif(std::vector<BDD>{ f });

    std::cout <<"\no DDcal:\n";	
    mgr.DumpDDcal(std::vector<BDD>{ f });

    //std::cout <<"\no Dot:\n";	
    //mgr.DumpDot(std::vector<BDD>{ f });

    //std::cout <<"\no DaVinci:\n";	
    //mgr.DumpDaVinci(std::vector<BDD>{ f });

    //std::cout <<"\no Form:\n";	
    //mgr.DumpFactoredForm(std::vector<BDD>{ f });

    std::cout <<"\no cout:\n" << f << '\n';

    //mgr.DumpFactoredForm(std::vector<BDD>{ f2 });
    //std::cout <<'\n' << f2 << '\n';
}