#include "include.h"
using namespace std;


string ResultStr;
vector<int> NodeList;
int NowUserId;
int PathCount;
bool IsServer[NODE_NUM_MAX];
unsigned char ServerId[NODE_NUM_MAX];
int supply[NODE_NUM_MAX];

void DFS(int v, int demand)
{
    NodeList.push_back(v);
    if (IsServer[v])
    {
        ++PathCount;
        ResultStr += "\n";
        for (int i = int(NodeList.size()) - 1; i >= 0; --i)
            ResultStr += to_string(NodeList[i]) + " ";
        ResultStr += to_string(NowUserId) + " ";

        if (supply[v] >= demand)
        {
            supply[v] -= demand;
            ResultStr += to_string(demand) + " ";
            ResultStr += to_string(ServerId[v]);
            return;
        }
        else
        {
            ResultStr += to_string(supply[v]) + " ";
            ResultStr += to_string(ServerId[v]);
            demand -= supply[v];
            supply[v] = 0;
        }
    }
    int given = 0;
    for (int i = 0, u, tmp, limit = int(Nodes[v].toSet.size()); i < limit; ++i)
    {
        u = Nodes[v].toSet[i];
        if (Nodes[u].flow[v] > 0 && given < demand)
        {
            if (Nodes[u].flow[v] >= demand - given) // u->v这条边能完全提供demand - given
            {
                Nodes[u].flow[v] -= demand - given;
                DFS(u, demand - given);
                NodeList.pop_back();
                break;
            }
            else                                    // u->v不能完全提供demand - given
            {
                tmp = Nodes[u].flow[v];
                Nodes[u].flow[v] = 0;
                given += tmp;
                DFS(u, tmp);
                NodeList.pop_back();
            }
        }
    }
}


// 使用DFS算法遍历网络得到所有path
string getPaths(vector<pair<int, unsigned char> > &servers, int *serverFlow)
{
    PathCount = 0;
    ResultStr = "";
    memset(IsServer, 0x00, NODE_NUM_MAX);
    for (auto &s: servers)
    {
        IsServer[s.first] = true;
        ServerId[s.first] = s.second;
        supply[s.first] = serverFlow[s.first];
    }
    mcf.StoreFlow(true);

    // DFS for each user
    for (int i = 0; i < User::Num; ++i)
    {
        NowUserId = i;
        NodeList.clear();
        DFS(Users[i].node->id, Users[i].demand);
    }

    return to_string(PathCount) + "\n" + ResultStr;
}