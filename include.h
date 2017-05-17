#ifndef CDN_INCLUDE_H
#define CDN_INCLUDE_H

#include <iostream>
#include <vector>
#include <string>
#include <deque>
#include <queue>
#include <utility>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <ctime>
using namespace std;

#define PRINT                           printf   //(...)//
#define str(var)                        #var" = %2d"
#define PRINT_1VAR(v0)                  printf(str(v0)"\n",v0)
#define PRINT_2VAR(v0, v1)              printf(str(v0)"\t" str(v1)"\n",v0,v1)
#define PRINT_3VAR(v0, v1, v2)          printf(str(v0)"\t" str(v1)"\t" str(v2)"\n",v0,v1,v2)
#define PRINT_4VAR(v0, v1, v2, v3)      printf(str(v0)"\t" str(v1)"\t" str(v2)"\t" str(v3)"\n",v0,v1,v2,v3)
#define PRINT_ERR(format, ...)          {PRINT( format, ##__VA_ARGS__); exit(0);}
#define GET_MIN(a, b)                   ((a) < (b) ? (a):(b))
#define GET_MAX(a, b)                   ((a) > (b) ? (a):(b))
#define ABS(x)                          ((x) >= 0 ? (x) : -(x))
#define NODE_NUM_MAX                    10002
#define USER_NUM_MAX                    10000
#define CAP_INF                         0x3F3F3F3F   // 一条边最大cap
#define COST_INF                        0x3F3F3F3F   // 一条边最大cost
#define TIME_OUT                        (clock() - StartTime > 88 * 1000000)
#define FEASIBLE                        -2
#define INFEASIBLE                      -1
#define PRECISION                       1

class Node;
class User;
class Serv;
class Tree;
class MCFSimplex;

extern unsigned long StartTime;
extern Node *Nodes, *Src, *Dst;
extern User *Users;
extern Serv Servs[10];
extern MCFSimplex mcf;

int simplex(int &Cost, int &Flow);
string getPaths(vector<pair<int, unsigned char> > &servers, int *serverFlow);
template<class T>
inline void Swap(T &a, T &b)
{
    T temp = a;
    a = b;
    b = temp;
}

#include "deploy.h"
#include "Node.h"
#include "Tree.h"
#include "MCFSimplex.h"

#endif