#ifndef CDN_TREE_H
#define CDN_TREE_H

using namespace std;

class Tree
{
public:
    static int Num;
    static Tree *Root;
    static inline int CreateRoot()
    {
        // set Root's totalCost, servers, serverFlow, fakeCost
        Root = new Tree(NULL);
        Root->totalCost = 0;
        for (Node *n = Nodes, *limit = Nodes + Node::Num - 2; n < limit; ++n)
            if (n->user)
            {
                int s_id = Serv::getId(n->user->demand);
                if (s_id == INFEASIBLE)
                    return INFEASIBLE;
                Root->totalCost += n->deployCost + Servs[s_id].cost;
                Root->servers.push_back(make_pair(n->id, s_id));
                Root->serverFlow[n->id] = n->user->demand;
                n->flow[Dst->id] = n->user->demand;
            }
        Root->fakeCost = Root->totalCost;

        // update best
        BestSln = Root;
        BestPaths = getPaths(Root->servers, Root->serverFlow);

        Sln_Insert(Root);
        return FEASIBLE;
    }

    static const int NoCandi;
    static vector<pair<int, int> > Prior;
    static void InitPrior();
    static inline bool cmp_less(const pair<int, int> &a, const pair<int, int> &b)
    { return a.second < b.second; }
    static inline bool cmp_more(const pair<int, int> &a, const pair<int, int> &b)
    { return a.second > b.second; }

    static vector<Tree *> Sln;
    static string BestPaths;
    static Tree *BestSln;
    static vector<unsigned int> HashRecord;

    // binary insert at HashRecord
    static inline void HashRecord_INSERT(unsigned int hash)
    {
        if (HashRecord.size() == 0)
            HashRecord.push_back(hash);
        int mid = 0, low = 0, high = int(HashRecord.size()) - 1;
        while (low <= high)
        {
            mid = (low + high) >> 1u;
            if (HashRecord[mid] < hash)
                low = mid + 1;
            else if (HashRecord[mid] > hash)
                high = mid - 1;
            else
                return;
        }
        if (HashRecord[mid] < hash)
            HashRecord.insert(HashRecord.begin() + mid + 1, hash);
        else if (HashRecord[mid] > hash)
            HashRecord.insert(HashRecord.begin() + mid, hash);
    }

    // binary search at HashRecord
    static inline bool HashRecord_Has(unsigned int hash)
    {
        int mid, low = 0, high = int(HashRecord.size()) - 1;
        while (low <= high)
        {
            mid = (low + high) >> 1u;
            if (HashRecord[mid] < hash)
                low = mid + 1;
            else if (HashRecord[mid] > hash)
                high = mid - 1;
            else
                return true;
        }
        return false;
    }

    // binary insert at Best(如果totalCost相等则不插入)
    static inline void Sln_Insert(Tree *t)
    {
        if (Sln.size() == 0) Sln.push_back(t);
        int mid = 0, low = 0, high = int(Sln.size()) - 1;
        while (low <= high)
        {
            mid = (low + high) >> 1u;
            if (Sln[mid]->totalCost < t->totalCost)
                low = mid + 1;
            else if (Sln[mid]->totalCost > t->totalCost)
                high = mid - 1;
            else
                return;
        }
        if (Sln[mid]->totalCost < t->totalCost)
            Sln.insert(Sln.begin() + mid + 1, t);
        else if (Sln[mid]->totalCost > t->totalCost)
            Sln.insert(Sln.begin() + mid, t);
    }

    static inline void PrintBestSln()
    {
        printf("********************** Print Best Solution ********************************\n");
        printf("used time : %.2fs\n", (clock() - StartTime) / 1000 / 1000.0);
        printf("BestTotalCost = %d\n", BestSln->totalCost / PRECISION);
        printf("BestServers:");
        for (auto &s : BestSln->servers)
            printf("%d(%d),", s.first, s.second);
        printf("\n");
    }


public:
    int id, level;
    Tree *father;
    unordered_set<int> tryCandis;         // 换成vector!!!
    unordered_map<int, Tree *> sons;        // 换成vector!!!

    int fakeCost, totalCost, flow;
    vector<pair<int, unsigned char> > servers;
    int *serverFlow;
    unsigned int hash;

    void resumeSrc(int flag, int candi, int src = 0, int dst = 0);
    void storeResult(bool);
    void tryFindBetter();
    Tree(Tree *father) : father(father)
    {
        id = Num++;
        level = (father == NULL) ? 0 : (father->level + 1);
        serverFlow = new int[Node::Num];
    }

    // 从Prior中得到第一个"未选择过的"候选点
    inline int getFirstCandi(int priorLimit)
    {
        int limit = GET_MIN(int(Prior.size()), priorLimit);
        for (int i = 0; i < limit; ++i)
            if (tryCandis.find(Prior[i].first) == tryCandis.end())
                return Prior[i].first;
        return NoCandi;
    }

    static inline bool Candi_Has(int candi)
    {
        for (auto c: Prior)
            if (candi == c.first)
                return true;
        return false;
    }

    // binary search at servers
    inline bool servers_Has(int s)
    {
        int mid, low = 0, high = int(servers.size()) - 1;
        while (low <= high)
        {
            mid = (low + high) >> 1u;
            if (servers[mid].first < s)
                low = mid + 1;
            else if (servers[mid].first > s)
                high = mid - 1;
            else
                return true;
        }
        return false;
    }

    inline unsigned int BKDRHash()
    {
        unsigned int hash = 0, seed = 131;  // 31 131 1313 13131 131313 etc..
        for (int i = 0, s, c, limit = int(Src->toSet.size()); i < limit; ++i)
        {
            s = Src->toSet[i];
            hash = hash * seed + (s & 0x000000FF);
            hash = hash * seed + ((s & 0x0000FF00) >> 8u);
            hash = hash * seed + ((s & 0x00FF0000) >> 16u);
            hash = hash * seed + ((s & 0xFF000000) >> 24u);

            c = Src->cost[s];
            hash = hash * seed + (c & 0x000000FF);
            hash = hash * seed + ((c & 0x0000FF00) >> 8u);
            hash = hash * seed + ((c & 0x00FF0000) >> 16u);
            hash = hash * seed + ((c & 0xFF000000) >> 24u);
        }
        return (hash & 0x7FFFFFFF);
    }

    inline void printServers(string str)
    {
        PRINT("%s :\tTree %d(level=%d)\n\t\tfakeCost =%d\n\t\ttotalCost=%d\n\t\tservers:",
              str.c_str(), id, level, fakeCost, totalCost / PRECISION);
        for (auto &s : servers)
            PRINT("%d(%d),", s.first, s.second);
        PRINT("\n");
    }
};


#endif //CDN_TREE_H
