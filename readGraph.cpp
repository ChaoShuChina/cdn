#include "include.h"


// global variables
unsigned long StartTime;            // start time of program
Node *Nodes;                        // set of all nodes
Node *Src, *Dst;                    // super source & destination node
User *Users;                        // set of all users
Serv Servs[10];                     // set of all server's kind


// static class member
int Node::Num, Node::EdgesNum;
int User::Num, User::DemandSum;
int Serv::Num, Serv::MaxCap = -1, Serv::MaxCost = -1;


void readGraph(char *topo[MAX_EDGE_NUM])
{
    // read number
    int nodesNum, edgesNum;
    sscanf(topo[0], "%d %d %d", &nodesNum, &edgesNum, &User::Num);
    Node::EdgesNum = (edgesNum << 1u) + nodesNum + User::Num;
    Node::Num = nodesNum + 2; //include super source & destination node

    // create Nodes & Users & Src & Dst
    Nodes = new Node[Node::Num];
    for (int i = 0; i < Node::Num; ++i)
        Nodes[i].setInfo(i, Node::Num);
    Src = &Nodes[Node::Num - 2];
    Dst = &Nodes[Node::Num - 1];
    Users = new User[User::Num];

    // create Servs (id, cap, cost)
    int line = 2;
    Serv::Num = 0;
    for (int id, cap, cost; 3 == sscanf(topo[line], "%d %d %d", &id, &cap, &cost); ++line)
    {
        ++Serv::Num;
        Servs[id].setInfo(id, cap, cost * PRECISION);
    }

    // set Nodes[id].deployCost
    for (int id, cost, limit = line + Node::Num - 2; line++ < limit;)
    {
        sscanf(topo[line], "%d %d", &id, &cost);
        Nodes[id].deployCost = cost * PRECISION;
        Src->addEdge(id, 0, 0);
    }

    // add edges to Nodes
    for (int s, t, cap, cost, limit = line + edgesNum; line++ < limit;)
    {
        sscanf(topo[line], "%d %d %d %d", &s, &t, &cap, &cost);
        Nodes[s].addEdge(t, cap, cost * PRECISION);
        Nodes[t].addEdge(s, cap, cost * PRECISION);
    }

    // set user's info
    for (int u_id, n_id, demand, limit = line + User::Num; line++ < limit;)
    {
        sscanf(topo[line], "%d %d %d", &u_id, &n_id, &demand);
        Users[u_id].setInfo(u_id, &Nodes[n_id], demand);
        Nodes[n_id].addEdge(Dst->id, demand, 0);  //add edges from Nodes[n_id] to Dst
    }
}