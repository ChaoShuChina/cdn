#include "include.h"
using namespace std;


char *topo[MAX_EDGE_NUM];
string caseName;
string absoluteDir("/home/chao-shu/Self_Project/0networkall/cdn4.20/cdn/case/");
vector<string> relativeDir = {"primary/", "medium/", "advanced/"};
unordered_map<string, int> OptCost;                 // 最优解权值
unordered_map<string, unordered_map<int, unsigned char> > OptSers;  // 最优解对应的服务器


// 引入gurobi求出的解(肯定是最优的)
void importOptimal()
{
    // --------------------------------------- medium ---------------------------------------
    OptCost["medium/case0"]  =  198673;
    OptSers["medium/case0"] = {{0,4},{3,5},{21,1},{29,4},{47,4},{49,1},{50,4},{70,4},{71,5},{88,3},{135,4},{141,4},{178,4},{179,4},{187,4},{205,4},{214,3},{223,5},{224,4},{236,3},{254,4},{255,5},{269,2},{271,1},{282,4},{290,1},{301,4},{321,4},{335,0},{344,4},{345,2},{351,4},{352,4},{365,2},{384,2},{387,4},{392,4},{393,4},{395,4},{401,2},{413,1},{423,2},{431,4},{476,4},{478,4},{511,4},{523,4},{539,3},{546,4},{548,4},{578,4},{582,3},{590,4}};

    OptCost["medium/case1"]  =  176786;
    OptSers["medium/case1"] =  {{4,4},{31,5},{35,4},{60,4},{62,4},{90,4},{113,3},{129,3},{133,4},{145,2},{152,3},{162,4},{166,5},{186,5},{195,4},{239,3},{241,4},{257,3},{260,4},{286,5},{289,4},{306,5},{317,4},{337,5},{343,3},{346,4},{355,4},{358,3},{376,4},{396,4},{398,4},{412,5},{442,4},{488,4},{495,5},{505,4},{509,5},{512,4},{549,4},{552,4},{570,4},{572,3},{574,4},{579,4},{595,0}};

    OptCost["medium/case2"]  =  201499;
    OptSers["medium/case2"] = {{18,3},{38,4},{42,4},{48,4},{68,5},{77,4},{80,2},{81,3},{95,3},{97,5},{99,0},{103,3},{110,4},{111,4},{136,4},{144,3},{155,4},{157,4},{158,4},{188,1},{190,4},{235,4},{241,5},{256,4},{262,1},{272,5},{273,5},{277,5},{310,4},{314,4},{323,3},{325,4},{349,5},{385,3},{388,4},{389,4},{414,4},{420,1},{426,4},{430,4},{456,5},{474,4},{490,3},{492,2},{521,5},{522,4},{556,4},{560,3},{573,2},{577,4},{579,4},{590,4},{596,1}};

    OptCost["medium/case3"]  =  192291;
    OptSers["medium/case3"] =  {{0,4},{3,4},{6,5},{9,0},{65,5},{86,3},{101,3},{119,4},{141,2},{161,5},{162,3},{171,5},{172,4},{200,4},{225,5},{226,4},{249,4},{255,3},{273,4},{275,4},{279,5},{284,3},{293,4},{326,4},{329,4},{331,1},{342,3},{358,5},{359,3},{361,4},{400,4},{402,4},{432,4},{433,5},{434,4},{441,2},{450,4},{453,4},{457,4},{459,4},{463,3},{472,4},{491,5},{515,4},{517,5},{545,4},{551,2},{553,5},{561,4}};

    // --------------------------------------- advanced ---------------------------------------
    OptCost["advanced/case0"]  =  387126;
}


// print Tree::Prior
void printPrior(string caseName, bool printAll)
{
    printf("************************* print Prior ************************\n");
    int rank = 0;
    for (auto &p: Tree::Prior)
    {
        bool isServer = (OptSers[caseName].find(p.first) != OptSers[caseName].end());
        if (printAll || isServer)  //(node in Gurobi optimal Sln)
            printf("rank=%3d\tnode=%3d\tfakeCost=%7d\tisServer:%d\n", rank, p.first, p.second, 11111 * isServer);
        rank++;
    }
}