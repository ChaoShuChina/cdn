#ifndef CDN_NODE_H
#define CDN_NODE_H


class Node
{
public:
    static int Num;         // number of nodes
    static int EdgesNum;    // number of edges
public:
    int id;                 // node's id: [0, Node::Num-1]
    int deployCost = 0;     // deploy cost at this node: [0, 10000]
    int maxOut = 0;         // maximum out flow limit
    User *user = NULL;      // connected user (NULL: not connected)

    vector<int> toSet;      // set of connected node's id: [0, Node::Num]
    int *cap;               // capacity(bandwidth) of edge: [0, 100]
    int *cost;              // cost of per flow: [0, 100]
    int *flow;              // flow on the edge: [0, cap]

    inline void setInfo(int id, int size)  // set id & apply for memory
    {
        this->id = id;
        cap = new int[size];
        cost = new int[size];
        flow = new int[size];
    }

    inline void addEdge(int to, int cap, int cost)
    {
        this->toSet.push_back(to);
        this->cap[to] = cap;
        this->cost[to] = cost;
        this->flow[to] = 0;
        maxOut += cap;
    }

    inline void setEdge(int to, int cap, int cost)
    {
        this->cap[to] = cap;
        this->cost[to] = cost;
    }

    inline void clearCap()
    {
        for (int to: toSet)
            cap[to] = 0;
    }
};


class User
{
public:
    static int Num;         // number of users
    static int DemandSum;   // demand sum of all users
public:
    int id;                 // node's id: [0, User::Num-1]
    int demand;             // demand capacity(bandwidth): [0, 10000]
    Node *node;             // connected node

    inline void setInfo(int id, Node *node, int demand)
    {
        this->id = id;
        this->node = node;
        this->demand = demand;
        this->DemandSum += demand;

        node->user = this;
    }
};


class Serv
{
public:
    static int Num;         // number of server's kind
    static int MaxCap;      // maximum out capacity(bandwidth) of all kind of server
    static int MaxCost;     // maximum purchase cost of all kind of server

    static inline int getId(int cap)    // get minimum server kind id which satisfy cap
    {
        for (int i = 0; i < Num; ++i)
            if (cap <= Servs[i].cap)
                return i;
        return INFEASIBLE;
    }
public:
    int id;                 // server kind's id: [0, ServersNum-1]
    int cap;                // maximum out capacity(bandwidth): [0, 10000]
    int cost;               // purchase cost

    inline void setInfo(int id, int cap, int cost)
    {
        this->id = id;
        this->cap = cap;
        this->cost = cost;
        if (cap > MaxCap) MaxCap = cap;
        if (cost > MaxCost) MaxCost = cost;
    }
};


#endif //CDN_NODE_H
