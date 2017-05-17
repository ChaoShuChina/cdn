#include "include.h"
using namespace std;
int Tree::Num = 0;
Tree *Tree::Root;
const int Tree::NoCandi = -1;
vector<pair<int, int> > Tree::Prior;
vector<Tree *> Tree::Sln;
Tree *Tree::BestSln;
string Tree::BestPaths("");
vector<unsigned int> Tree::HashRecord;


// calculate all Nodes's priority, try deploy server at each node (punish cost from Src to candi = 0)
void Tree::InitPrior()
{
    for (int candi = 0, limit = Node::Num - 2; candi < limit && !TIME_OUT; ++candi)
    {
        Tree *leaf = new Tree(Root);

        Root->resumeSrc(0, candi);
        if (simplex(leaf->fakeCost, leaf->flow) == INFEASIBLE) continue;
        leaf->storeResult(true);

        Root->sons[candi] = leaf;
        if (leaf->serverFlow[candi] == 0)   continue;
        Prior.push_back(make_pair(candi, leaf->fakeCost));
    }
    sort(Prior.begin(), Prior.end(), cmp_less);
    //Prior.resize((unsigned long) GET_MAX(5, User::Num));
}


// 初始化从超级原点到{servers,candi}的虚拟边
#define n_id            s.first
#define s_id            s.second
#define node_punish0    ((int) round((Servs[s_id].cost) / float(serverFlow[n_id])))
#define node_punish1    ((int) round((Nodes[n_id].deployCost + Servs[s_id].cost) / float(serverFlow[n_id])))
#define candi_punish    (Nodes[candi].deployCost / GET_MIN(Serv::MaxCap, Nodes[candi].maxOut))
void Tree::resumeSrc(int flag, int candi, int remove1, int remove2)
{
    Src->clearCap();

    if (flag == 0)  // 无条件增加candi,有权值
    {
        for (auto &s : servers)
            Src->setEdge(n_id, Servs[s_id].cap, node_punish0);
        Src->setEdge(candi, Serv::MaxCap, candi_punish);
    }
    else if (flag == 1)// 无条件增加candi,有权值
    {
        for (auto &s : servers)
            Src->setEdge(n_id, Servs[s_id].cap, node_punish1);
        Src->setEdge(candi, Serv::MaxCap, candi_punish);
    }
    else if (flag == 2) // 无条件增加candi,全部权值设为0
    {
        for (auto &s : servers)
            Src->setEdge(n_id, Serv::MaxCap, node_punish1);
        Src->setEdge(candi, Serv::MaxCap, 0);
    }
    else if (flag == 3) // 在servers中去除candi
    {
        for (auto &s : servers)
            Src->setEdge(n_id, Serv::MaxCap, 0);
        Src->setEdge(candi, 0, candi_punish);
    }
    else if (flag == 4)
    {
        for (auto &s : servers)
            Src->setEdge(n_id, Serv::MaxCap, 0);
    }
    else if (flag == 7) // 无条件增加candi,一次增加两个点
    {
        for (auto &s : servers)
            Src->setEdge(n_id, Serv::MaxCap, node_punish1);
        Src->setEdge(candi, Serv::MaxCap, 0);
        Src->setEdge(remove1, Serv::MaxCap, 0);
    }
    else if (flag == 8) // 在servers中去除candi
    {
        for (auto &s : servers)
            Src->setEdge(n_id, Serv::MaxCap, 0);
        Src->setEdge(candi, 0, candi_punish);
        Src->setEdge(remove1, 0, candi_punish);
    }
    else if (flag == 9) // 一次加三个点
    {
        for (auto &s : servers)
            Src->setEdge(n_id, Serv::MaxCap, 0);
        Src->setEdge(candi, 0, candi_punish);
        Src->setEdge(remove1, 0, candi_punish);
        Src->setEdge(remove2, 0, candi_punish);
    }
    if (flag == 5 || flag == 6)
    {
        for (auto &s : servers)
            Src->setEdge(n_id, Serv::MaxCap, node_punish1);
        if (flag == 5)
        {
            Src->setEdge(candi, Serv::MaxCap, candi_punish);
            Src->setEdge(remove1, 0, 0);
        }
        else if (flag == 6)
        {
            Src->setEdge(candi, Serv::MaxCap, candi_punish);
            Src->setEdge(remove1, 0, 0);
            Src->setEdge(remove2, 0, 0);
        }
    }


}
#undef n_id
#undef s_id
#undef node_punish0
#undef node_punish1
#undef candi_punish


// 存储使用MCMF后的服务器布局信息,如果更优则更新最优解
void Tree::storeResult(bool _tryFindBetter)
{
    totalCost = fakeCost;
    if (servers.size() > 0) servers.clear();

    for (int to : Src->toSet)
        if (Src->flow[to] > 0)
        {
            int s_id = Serv::getId(Src->flow[to]);
            if (s_id == INFEASIBLE)
            {
                cout << "s_id INFEASIBLE! At function storeResult()\n";
                fakeCost = totalCost = INT32_MAX;
                servers.clear();
                break;
            }
            totalCost -= Src->cost[to] * Src->flow[to];
            totalCost += Nodes[to].deployCost + Servs[s_id].cost;
            servers.push_back(make_pair(to, s_id));
            serverFlow[to] = Src->flow[to];
        }

    if (totalCost < BestSln->totalCost)
    {
        BestSln = this;
        BestPaths = getPaths(servers, serverFlow);
    }

    if (_tryFindBetter)
    {
        if (totalCost < Sln[0]->totalCost)
            tryFindBetter();
        HashRecord_INSERT(hash);
        Sln_Insert(this);
    }
}


// store better
void Tree::tryFindBetter()
{
    // 设置从src到当前最优方案服务器的cost为0,再重新求一遍
    int _flow, _totalCost;
    int _serverFlow[Node::Num];
    vector<pair<int, unsigned char> > _servers;

    resumeSrc(4, -1);
    if (INFEASIBLE == simplex(_totalCost, _flow)) return;
    for (int to : Src->toSet)
        if (Src->flow[to] > 0)
        {
            int s_id = Serv::getId(Src->flow[to]);
            if (s_id == INFEASIBLE)
            {
                cout << "s_id INFEASIBLE! At function storeBetter()\n";
                _totalCost = INT32_MAX;
                break;
            }
            _totalCost += Nodes[to].deployCost + Servs[s_id].cost;
            _servers.push_back(make_pair(to, s_id));
            _serverFlow[to] = Src->flow[to];
        }

    // update best
    if (_totalCost < totalCost)
    {
        totalCost = _totalCost;
//        servers = _servers;
        BestPaths = getPaths(_servers, _serverFlow);
    }
}