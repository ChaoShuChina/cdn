#include "include.h"
#include<cmath>

using namespace std;


static bool isVis[NODE_NUM_MAX];
static int dis[NODE_NUM_MAX];
static int root[NODE_NUM_MAX];

struct fourElem {
    int u, v, mid, dis;

    fourElem(int u, int v, int mid, int dis) : u(u), v(v), mid(mid), dis(dis) {};

    static inline bool cmp_less(const fourElem &a, const fourElem &b) {
        if (a.dis < b.dis)
            return true;
        else if (a.dis == b.dis) {
            if (a.u < b.u)
                return true;
            else if (a.u == b.u && a.v < b.v)
                return true;
        }
        return false;
    }
};


void BFS(vector<fourElem> &vec, Tree *t) {
    deque<int> Q1, Q2;
    deque<int> *father = &Q1, *son = &Q2;

    memset(isVis, 0x00, Node::Num * sizeof(bool));
    memset(dis, 0x00, Node::Num * sizeof(int));
    vec.clear();
    for (auto &s: t->servers) {
        isVis[s.first] = true;
        father->push_back(s.first);
        root[s.first] = s.first;
    }

    while (!(father->empty())) {
        while (!(father->empty())) {
            int id = father->front();
            father->pop_front();
            for (int to: Nodes[id].toSet) {
                if (to == Dst->id)
                    continue;
                if (isVis[to]) {
                    if (root[id] < root[to]) {
                        vec.push_back(fourElem(root[id], root[to], to, dis[id] + dis[to] + 1));
                        int d = dis[id] + dis[to] + 1;
//                        cout << id << " " << to << " ";
//                        PRINT_4VAR(root[id], root[to], to, d);
                    }
                } else {
                    isVis[to] = true;
                    son->push_back(to);
                    root[to] = root[id];
                    dis[to] = dis[id] + 1;
                }
            }
        }
        Swap(father, son);
    }
    sort(vec.begin(), vec.end(), fourElem::cmp_less);
}


inline void solve(Tree *t, Tree *leaf) {
    leaf = new Tree(t);
    if (simplex(leaf->fakeCost, leaf->flow) != INFEASIBLE) {
        leaf->storeResult(false);
        PRINT_1VAR(leaf->totalCost);
        // leaf->printServers("");
    }
}

void mergerNode() {
    vector<fourElem> vec;
    for (int run = 0; run < 1 && !TIME_OUT; ++run) {
        Tree *t = Tree::BestSln, *leaf = NULL;
        //PRINT_1VAR(t->totalCost);
        BFS(vec, t);
        for (int i = 0; i < vec.size() && !TIME_OUT; ++i) {
            int u = vec[i].u, v = vec[i].v, mid = vec[i].mid, dis = vec[i].dis;
            PRINT_4VAR(u, v, mid, dis);

//            t->resumeSrc(5, mid, u, -1), solve(t, leaf);
//            t->resumeSrc(5, mid, v, -1), solve(t, leaf);
//            if (mid != u && mid != v) t->resumeSrc(6, mid, u, v), solve(t, leaf);
//            if (leaf == Tree::BestSln)
//                break;
        }
    }
}

void SimulatedAnnealing() {
    Tree *bt, *leaf;
    float dE = 0, chance;
    float T = 10000;//初始化温度
    float T_time = 0.00001;
    int mm = 0, nn = 0;
    for (bt = Tree::BestSln; T > T_time && !TIME_OUT; ) {
        float P = rand() / float(INT32_MAX);
        leaf = new Tree(bt);
        if (rand() % 2 == 0) {   //若随机数为偶数，选择加点
            int candi1 = rand() % (Node::Num - 2);
            int candi2 = rand() % (Node::Num - 2);
//            if (bt->servers_Has(candi))
//                continue;
            if (!Tree::Candi_Has(candi1))
                continue;
//            if (!Tree::Candi_Has(candi2))
//                continue;
            bt->resumeSrc(7, candi1, candi2);
//            bt->resumeSrc(2, candi1);
        } else {//为奇数则删除点
            auto m = rand() % bt->servers.size();
            auto n = rand() % bt->servers.size();
            int candi1 = bt->servers[m].first;
            int candi2 = bt->servers[n].first;
            if (Tree::Candi_Has(candi1))
                continue;
//            if (Tree::Candi_Has(candi2))
//                continue;
            bt->resumeSrc(8, candi1, candi2);
        }
        if (simplex(leaf->fakeCost, leaf->flow) == FEASIBLE) {
            leaf->storeResult(false);
            PRINT_1VAR(Tree::BestSln->totalCost);

            mm++;
            if (mm - nn > 25) {//升温策略，连续被拒绝十次之后，升温0.1
                T = T * 1.01;
                mm = 0;
                nn = 0;
            }

            if (leaf->totalCost < bt->totalCost) {   // have find better;
                bt = leaf;
                T = T * 0.999;
                nn++;
            } else {
                dE = bt->totalCost - leaf->totalCost;
                chance = exp(dE / T);
                cout << "chance = " << chance << "\n";
                if (P < chance) {
                    bt = leaf;
                    nn++;
                }
            }
        }
//        else{
//
//        }
    }
}

void deploy_server(char *topo[MAX_EDGE_NUM], int line_num, const char *filename) {
    StartTime = clock();
    readGraph(topo);
    mcf.Initial();

    if (Tree::CreateRoot() == INFEASIBLE) {
        write_result("NA", filename);
        return;
    }
    Tree::InitPrior();
    SimulatedAnnealing();
//    BranchBound();
//    LocalSearch();

//    mergerNode();

//
//    cout<<Node::Num<<endl;
//    Tree *bt = Tree::BestSln;
//    Tree *leaf = new Tree(bt);
//
//    bt->resumeSrc(2, ?); // 2:+, 3:-
//    if (simplex(leaf->fakeCost, leaf->flow) == FEASIBLE)
//    {
//        leaf->storeResult(false);
//    }


    write_result(Tree::BestPaths.c_str(), filename);
}

void BranchBound() {
    int runLimit = 2000, priorLimit = GET_MIN(15, int(Tree::Prior.size()));
    Tree *t = Tree::Root, *leaf;

    for (int run = 0, candi; t && run < runLimit && !TIME_OUT; ++run) {
        if ((candi = t->getFirstCandi(priorLimit)) == Tree::NoCandi) {
            t = t->father;
            continue;
        }

        if (t != Tree::Root) {
            leaf = new Tree(t);
            t->tryCandis.insert(candi);
            t->sons[candi] = leaf;

            // simplex algorithm
            t->resumeSrc(1, candi);
            if (Tree::HashRecord_Has(leaf->hash = leaf->BKDRHash())) continue;
            if (simplex(leaf->fakeCost, leaf->flow) == INFEASIBLE) continue;
            leaf->storeResult(true);
        } else {
            t->tryCandis.insert(candi);
            leaf = t->sons[candi];
        }
//        leaf->printServers("");

        if (leaf->fakeCost < t->fakeCost || leaf->totalCost < t->totalCost)
            t = leaf;
    }
}


// 在最优解附近搜索(加点或减点)
void LocalSearch() {
    static unordered_set<Tree *> Sln0;
    vector<Tree *> _Sln = Tree::Sln;
    for (int i = 0; i < 6; ++i) {
        if (i > 0) Tree::Sln = _Sln;
        for (int j = 0; j < i; ++j) {
            auto _servers = Tree::Sln[0]->servers;
            while (1)
                if (Tree::Sln[0]->servers == _servers)
                    Tree::Sln.erase(Tree::Sln.begin());
                else
                    break;
        }

        // 如果曾经从Tree::Sln[0]开始LocalSearch过就continue
        if (Sln0.find(Tree::Sln[0]) == Sln0.end())
            Sln0.insert(Tree::Sln[0]);
        else
            continue;

        // LocalSearch: AddNode & RemoveNode
        Tree *lastBest;
        for (int run = 0; run < 15 && !(TIME_OUT); ++run) {
            lastBest = Tree::Sln[0];
            AddNode();
            RemoveNode();
            if (lastBest->totalCost == Tree::Sln[0]->totalCost) {
                printf("local search round : %d\n", run + 1);
                break;
            }
        }
    }
}


// 寻找丢失的"最优服务器"
void AddNode() {
    Tree *bt, *leaf;
    for (int i = 0, limit = int(Tree::Prior.size()); i < limit && !TIME_OUT; ++i) {
        bt = Tree::Sln[0];   // 有bug
        int s = Tree::Prior[i].first;
        if (!bt->servers_Has(s)) {
            leaf = new Tree(bt);
            bt->resumeSrc(2, s);
            if (Tree::HashRecord_Has(leaf->hash = leaf->BKDRHash())) continue;
            if (simplex(leaf->fakeCost, leaf->flow) == INFEASIBLE) continue;
            leaf->storeResult(true);
        }
    }
}


// 减少"非最优服务器"
void RemoveNode() {
    Tree *bt = Tree::Sln[0], *leaf;
    for (int i = 0, limit = int(bt->servers.size()); i < limit && !(TIME_OUT); ++i) {
        leaf = new Tree(bt);
        bt->resumeSrc(3, bt->servers[i].first);
        if (Tree::HashRecord_Has(leaf->hash = leaf->BKDRHash())) continue;
        if (simplex(leaf->fakeCost, leaf->flow) == INFEASIBLE) continue;
        leaf->storeResult(true);
    }
}