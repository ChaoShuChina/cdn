#ifndef CDN_CDN_H
#define CDN_CDN_H


extern char *topo[MAX_EDGE_NUM];
extern string caseName;
extern string absoluteDir;
extern vector<string> relativeDir;
extern unordered_map<string, int> OptCost;
extern unordered_map<string, unordered_map<int, unsigned char>> OptSers;


void compareGurobi(string caseName);
void importOptimal();
void printPrior(string caseName, bool printAll);


#endif //CDN_CDN_H
