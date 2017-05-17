#include "include.h"
#include "cdn.h"
using namespace std;


int main(int argc, char *argv[])
{
    if (argc == 1)
    {
//        caseName = relativeDir[0] + "case00";
//        caseName = "primary/case00";
        caseName = "medium/case0";
//        caseName = "advanced/case0";
    }
    else
    {
        caseName = string(argv[1]).substr(absoluteDir.size());
        caseName = caseName.substr(0, caseName.size() - 4);
    }
    importOptimal();
    int line_num = read_file(topo, MAX_EDGE_NUM, (absoluteDir + caseName + ".txt").c_str());
    deploy_server(topo, line_num, (absoluteDir + caseName + "_result.txt").c_str());

    Tree::PrintBestSln();
//    printPrior(caseName, false);
    compareGurobi(caseName);
    return 0;
}


// compare with Gurobi optimal solution
void compareGurobi(string caseName)
{
    printf("************************* compare with Gurobi ************************\n");
    printf("Num of Gurobi's servers: %ld\n", OptSers[caseName].size());

    printf("Excessive: ");
    for (auto &s : Tree::BestSln->servers)
        if (OptSers[caseName].find(s.first) == OptSers[caseName].end())
            printf("%d,", s.first);

    printf("\nLack of: ");
    for (auto &s: OptSers[caseName])
        if (!Tree::BestSln->servers_Has(s.first))
            printf("%d,", s.first);

    // cost difference
    int optDelta = Tree::BestSln->totalCost / PRECISION - OptCost[caseName];
    printf("\nGap = %.2f%% (%d), Own = %d, Optimal = %d\n", optDelta * 100.0 / OptCost[caseName], optDelta,
           Tree::BestSln->totalCost / PRECISION, OptCost[caseName]);
}
