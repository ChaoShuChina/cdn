#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "lib_io.h"

void deploy_server(char * graph[MAX_EDGE_NUM], int edge_num, const char * filename);
void readGraph(char *topo[MAX_EDGE_NUM]);

void BranchBound();
void LocalSearch();
void AddNode();
void RemoveNode();

#endif
