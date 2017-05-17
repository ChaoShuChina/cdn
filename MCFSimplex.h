#ifndef __MCFSimplex
#define __MCFSimplex  /* self-identification: #endif at the end of the file */

class MCFSimplex
{
public:
    int n, nmax, m, mmax, status;
    enum MCFStatus
    {
        kUnSolved = -1, kOK = 0, kUnfeasible, kUnbounded,
    };
    void Initial();
    void updateGraph();
    void SolveMCF();
    int GetCost();
    void StoreFlow(bool);
private:
    struct arcPType;   // pre-declaration of the arc structure (pointers to arcs
    struct nodePType
    {
        nodePType *prevInT;     // previous node in the order of the Post-Visit on T
        nodePType *nextInT;     // next node in the order of the Post-Visit on T
        arcPType *enteringTArc; // entering basic arc of this node
        int balance;        // supply/demand of this node; a node is called a
        int potential;      // the node potential corresponding with the flow
        int subTreeLevel;        // the depth of the node in T as to T root

    };                       // end( struct( nodePType ) )
    struct arcPType
    {
        nodePType *tail;        // tail node
        nodePType *head;        // head node
        int flow;           // arc flow
        int cost;           // arc linear cost
        char ident;            // tells if arc is deleted, closed, in T, L, or U
        int upper;          // arc upper bound
    };                      // end( struct( arcPType ) )
    struct primalCandidType
    {
        arcPType *arc;            // pointer to the violating primal bound arc
        int absRC;           // absolute value of the arc's reduced cost
    };                        // end( struct( primalCandidateType ) )
    nodePType *nodesP;             // vector of nodes: points to the n + 1 node
    nodePType *dummyRootP;         // the dummy root node
    nodePType *stopNodesP;         // first infeasible node address = nodes + n
    arcPType *arcsP;               // vector of arcs; this variable points to
    arcPType *dummyArcsP;          // vector of artificial dummy arcs: points
    arcPType *stopArcsP;           // first infeasible arc address = arcs + m
    arcPType *stopDummyP;          // first infeasible dummy arc address
    int iterator;         // the current number of iterations
    primalCandidType *candP;       // every element points to an element of the
    int numGroup;                // number of the candidate lists
    int tempCandidateListSize;   // hot list dimension (it is variable)
    int groupPos;                // contains the actual candidate list
    int numCandidateList;        // number of candidate lists
    int hotListSize;             // number of candidate lists and hot list dimension
    int forcedNumCandidateList;  // value used to force the number of candidate list
    int forcedHotListSize;       // value used to force the number of candidate list
    int MAX_ART_COST;          // large cost for artificial arcs

    void MemAllocCandidateList(void);
    void CreateInitialPrimalBase(void);
    void PrimalSimplex(void);
    template<class N, class A>
    void UpdateT(A *h, A *k, N *h1, N *h2, N *k1, N *k2);
    template<class N>
    N *CutAndUpdateSubtree(N *root, int delta);
    template<class N>
    void PasteSubtree(N *root, N *lastNode, N *previousNode);
    arcPType *RulePrimalCandidateListPivot(void);
    inline void InitializePrimalCandidateList(void);
    inline void SortPrimalCandidateList(int min, int max);
    template<class N, class RCT>
    inline void AddPotential(N *r, RCT delta);
    template<class N, class A>
    inline N *Father(N *n, A *a);
};
#endif