#include "include.h"

#define ReductCost(a) (((a)->tail)->potential - ((a)->head)->potential + (a)->cost)

static const char BASIC = 0;  // ident for basis arcs
static const char AT_LOWER = 1;  // ident for arcs in L
static const char AT_UPPER = 2;  // ident for arcs in U
static const int PRIMAL_LOW_NUM_CANDIDATE_LIST = 30;
static const int PRIMAL_MEDIUM_NUM_CANDIDATE_LIST = 50;
static const int PRIMAL_HIGH_NUM_CANDIDATE_LIST = 200;
static const int PRIMAL_LOW_HOT_LIST_SIZE = 5;
static const int PRIMAL_MEDIUM_HOT_LIST_SIZE = 10;
static const int PRIMAL_HIGH_HOT_LIST_SIZE = 20;
MCFSimplex mcf;

int simplex(int &Cost, int &Flow)
{
    mcf.updateGraph();
    mcf.SolveMCF();
    if (mcf.status == MCFSimplex::kOK)
    {
        Cost = mcf.GetCost();
        Flow = User::DemandSum;
//        cout << "OptCost = " << Cost << "\t\tFlow = " << Flow << endl;
        mcf.StoreFlow(false);
        return FEASIBLE;
    }
    else
    {
        cout << "MCF problem unfeasible." << endl;
        return INFEASIBLE;
    }
}

void MCFSimplex::Initial()
{
    n = nmax = Node::Num;
    m = mmax = Node::EdgesNum;
    forcedNumCandidateList = 0;
    forcedHotListSize = 0;
    MAX_ART_COST = int(1e7);

    nodesP = new nodePType[nmax + 1];   // array of nodes
    arcsP = new arcPType[mmax + nmax];  // array of arcs
    dummyArcsP = arcsP + mmax;            // artificial arcs are in the last
    stopNodesP = nodesP + n;
    dummyRootP = nodesP + nmax;
    stopDummyP = dummyArcsP + n;
    stopArcsP = arcsP + m;

    // add edges
    arcPType *arc = arcsP;
    for (int to: Src->toSet)
    {
        arc->tail = nodesP + Src->id;
        arc->head = nodesP + to;
        arc->upper = Src->cap[to];
        arc->cost = Src->cost[to];
        arc++;
    }
    for (int i = 0; i < Node::Num - 2; ++i)
        for (int to: Nodes[i].toSet)
        {
            arc->tail = nodesP + i;
            arc->head = nodesP + to;
            arc->upper = Nodes[i].cap[to];
            arc->cost = Nodes[i].cost[to];
            arc++;
        }

    // add supply
    for (nodePType *node = nodesP; node != stopNodesP; node++)
        node->balance = 0;
    (nodesP + Src->id)->balance = -User::DemandSum;
    (nodesP + Dst->id)->balance = User::DemandSum;

    MemAllocCandidateList();
}

void MCFSimplex::updateGraph()
{
    arcPType *arc = arcsP;
    for (int to: Src->toSet)
    {
        arc->upper = Src->cap[to];
        arc->cost = Src->cost[to];
        arc++;
    }
    status = kUnSolved;
}

void MCFSimplex::SolveMCF(void)
{
    CreateInitialPrimalBase();
    PrimalSimplex();
}

#define s ((arcsP + i)->tail - nodesP)
#define t ((arcsP + i)->head - nodesP)
void MCFSimplex::StoreFlow(bool all)
{
    int limit = all ? m : int(Src->toSet.size());
    for (int i = 0; i < limit; i++)
        Nodes[s].flow[t] = (arcsP + i)->flow;
}
#undef s
#undef t


void MCFSimplex::MemAllocCandidateList(void)
{
    if (m < 10000)
    {
        numCandidateList = PRIMAL_LOW_NUM_CANDIDATE_LIST;
        hotListSize = PRIMAL_LOW_HOT_LIST_SIZE;
    }
    else if (m > 100000)
    {
        numCandidateList = PRIMAL_HIGH_NUM_CANDIDATE_LIST;
        hotListSize = PRIMAL_HIGH_HOT_LIST_SIZE;
    }
    else
    {
        numCandidateList = PRIMAL_MEDIUM_NUM_CANDIDATE_LIST;
        hotListSize = PRIMAL_MEDIUM_HOT_LIST_SIZE;
    }

    if (forcedNumCandidateList > 0)
        numCandidateList = forcedNumCandidateList;

    if (forcedHotListSize > 0)
        hotListSize = forcedHotListSize;

    delete[] candP;
    candP = new primalCandidType[hotListSize + numCandidateList + 1];
}

void MCFSimplex::CreateInitialPrimalBase(void)
{
    arcPType *arc;
    nodePType *node;
    for (arc = arcsP; arc != stopArcsP; arc++)
    {  // initialize real arcs
        arc->flow = 0;
        arc->ident = AT_LOWER;
    }

    for (arc = dummyArcsP; arc != stopDummyP; arc++)
    {  // initialize dummy arcs
        node = nodesP + (arc - dummyArcsP);
        if (node->balance > 0)
        {  // sink nodes
            arc->tail = dummyRootP;
            arc->head = node;
            arc->flow = node->balance;
        }
        else
        {  // source nodes or transit node
            arc->tail = node;
            arc->head = dummyRootP;
            arc->flow = -node->balance;
        }

        arc->cost = MAX_ART_COST;
        arc->ident = BASIC;
        arc->upper = INT32_MAX;
    }

    dummyRootP->balance = 0;
    dummyRootP->prevInT = NULL;
    dummyRootP->nextInT = nodesP;
    dummyRootP->enteringTArc = NULL;
    dummyRootP->potential = MAX_ART_COST;
    dummyRootP->subTreeLevel = 0;
    for (node = nodesP; node != stopNodesP; node++)
    {  // initialize nodes
        node->prevInT = node - 1;
        node->nextInT = node + 1;
        node->enteringTArc = dummyArcsP + (node - nodesP);
        if (node->balance > 0)  // sink nodes
            node->potential = 2 * MAX_ART_COST;
        else                     // source nodes or transit node
            node->potential = 0;

        node->subTreeLevel = 1;
    }

    nodesP->prevInT = dummyRootP;
    (nodesP + n - 1)->nextInT = NULL;
}

void MCFSimplex::PrimalSimplex(void)
{
    status = kUnSolved;
    iterator = 0;  // setting the initial arc for the Dantzig or First Elibigle Rule

    arcPType *enteringArc;
    arcPType *leavingArc;
    InitializePrimalCandidateList();

    while (status == kUnSolved)
    {
        iterator++;
        enteringArc = RulePrimalCandidateListPivot();
        if (enteringArc)
        {
            arcPType *arc;
            nodePType *k1;
            nodePType *k2;

            int t;
            int theta;
            if (enteringArc->ident == AT_UPPER)
            {
                k1 = enteringArc->head;
                k2 = enteringArc->tail;
                theta = enteringArc->flow;
            }
            else
            {
                k1 = enteringArc->tail;
                k2 = enteringArc->head;
                theta = enteringArc->upper - enteringArc->flow;
            }

            nodePType *memK1 = k1;
            nodePType *memK2 = k2;
            leavingArc = NULL;
            bool leavingReducesFlow = (ReductCost(enteringArc) > 0);
            // Compute "theta", find outgoing arc and "root" of the cycle
            bool leave;
            // Actual "theta" is compared with the bounds of the other cycle's arcs
            while (k1 != k2)
            {
                if (k1->subTreeLevel > k2->subTreeLevel)
                {
                    arc = k1->enteringTArc;
                    if (arc->tail != k1)
                    {
                        t = arc->upper - arc->flow;
                        leave = false;
                    }
                    else
                    {
                        t = arc->flow;
                        leave = true;
                    }

                    if (t < theta)
                    {
                        // The algorithm has found a possible leaving arc
                        theta = t;
                        leavingArc = arc;
                        leavingReducesFlow = leave;
                        // If "leavingReducesFlow" == true, if this arc is selected to exit the base,
                        // it decreases its flow
                    }

                    k1 = Father(k1, arc);
                }
                else
                {
                    arc = k2->enteringTArc;
                    if (arc->tail == k2)
                    {
                        t = arc->upper - arc->flow;
                        leave = false;
                    }
                    else
                    {
                        t = arc->flow;
                        leave = true;
                    }

                    if (t <= theta)
                    {
                        // The algorithm has found a possible leaving arc
                        theta = t;
                        leavingArc = arc;
                        leavingReducesFlow = leave;
                        // If "leavingReducesFlow" == true, if this arc is selected to exit the base,
                        // it decreases its flow
                    }

                    k2 = Father(k2, arc);
                }
            }

            if (leavingArc == NULL)
                leavingArc = enteringArc;

            if (theta >= INT32_MAX)
            {
                status = kUnbounded;
                break;
            }

            // Update flow with "theta"
            k1 = memK1;
            k2 = memK2;
            if (theta != 0)
            {
                if (enteringArc->tail == k1)
                    enteringArc->flow = enteringArc->flow + theta;
                else
                    enteringArc->flow = enteringArc->flow - theta;

                while (k1 != k2)
                {
                    if (k1->subTreeLevel > k2->subTreeLevel)
                    {
                        arc = k1->enteringTArc;
                        if (arc->tail != k1)
                            arc->flow = arc->flow + theta;
                        else
                            arc->flow = arc->flow - theta;

                        k1 = Father(k1, k1->enteringTArc);
                    }
                    else
                    {
                        arc = k2->enteringTArc;
                        if (arc->tail == k2)
                            arc->flow = arc->flow + theta;
                        else
                            arc->flow = arc->flow - theta;

                        k2 = Father(k2, k2->enteringTArc);
                    }
                }
            }

            if (enteringArc != leavingArc)
            {
                bool leavingBringFlowInT2 = (leavingReducesFlow ==
                                             ((leavingArc->tail)->subTreeLevel > (leavingArc->head)->subTreeLevel));
                // "leavingBringFlowInT2" == true if leaving arc brings flow to the subtree T2
                if (leavingBringFlowInT2 == (memK1 == enteringArc->tail))
                {
                    k2 = enteringArc->tail;
                    k1 = enteringArc->head;
                }
                else
                {
                    k2 = enteringArc->head;
                    k1 = enteringArc->tail;
                }
            }
            if (leavingReducesFlow)
                leavingArc->ident = AT_LOWER;
            else
                leavingArc->ident = AT_UPPER;

            if (leavingArc != enteringArc)
            {
                enteringArc->ident = BASIC;
                nodePType *h1;
                nodePType *h2;
                // "h1" is the node in the leaving arc with smaller tree's level
                if ((leavingArc->tail)->subTreeLevel < (leavingArc->head)->subTreeLevel)
                {
                    h1 = leavingArc->tail;
                    h2 = leavingArc->head;
                }
                else
                {
                    h1 = leavingArc->head;
                    h2 = leavingArc->tail;
                }

                UpdateT(leavingArc, enteringArc, h1, h2, k1, k2);
                // Update potential of the subtree T2
                k2 = enteringArc->head;
                int delta = ReductCost(enteringArc);
                if ((enteringArc->tail)->subTreeLevel > (enteringArc->head)->subTreeLevel)
                {
                    delta = -delta;
                    k2 = enteringArc->tail;
                }

                AddPotential(k2, delta);
                // In the linear case Primal Simplex only updates the potential of the nodes of
                // subtree T2
            }
        }
        else
        {
            status = kOK;
            // If one of dummy arcs has flow bigger than 0, the solution is unfeasible.
            for (arcPType *arc = dummyArcsP; arc != stopDummyP; arc++)
                if (arc->flow > 0)
                    status = kUnfeasible;
        }
    }
}  // end( PrimalSimplex )

/*--------------------------------------------------------------------------*/

template<class N, class A>
void MCFSimplex::UpdateT(A *h, A *k, N *h1, N *h2, N *k1, N *k2)
{
    int delta = (k1->subTreeLevel) + 1 - (k2->subTreeLevel);
    N *root = k2;
    N *dad;
    N *previousNode = k1;
    N *lastNode;
    A *arc1 = k;
    A *arc2;
    bool fine = false;
    while (fine == false)
    {
        // If analysed node is h2, this is the last iteration
        if (root == h2)
            fine = true;

        dad = Father(root, root->enteringTArc);
        // Cut the root's subtree from T and update the "subLevelTree" of its nodes
        lastNode = CutAndUpdateSubtree(root, delta);
        // Paste the root's subtree in the right position;
        PasteSubtree(root, lastNode, previousNode);
        // In the next iteration the subtree will be moved beside the last node of
        // the actual analysed subtree.
        previousNode = lastNode;
        /* The increase of the subtree in the next iteration is different from
     the actual increase: in particual the increase increases itself (+2
     at every iteration). */
        delta = delta + 2;
        /* A this point "enteringTArc" of actual root is stored in "arc2" and
     changed; then "arc1" and "root" are changed. */
        arc2 = root->enteringTArc;
        root->enteringTArc = arc1;
        arc1 = arc2;
        root = dad;
    }
}

template<class N>
N *MCFSimplex::CutAndUpdateSubtree(N *root, int delta)
{
    int level = root->subTreeLevel;
    N *node = root;
    // The root of this subtree is passed by parameters, the last node is searched.
    while ((node->nextInT) && ((node->nextInT)->subTreeLevel > level))
    {
        node = node->nextInT;
        // The "subTreeLevel" of every nodes of subtree is updated
        node->subTreeLevel = node->subTreeLevel + delta;
    }

    root->subTreeLevel = root->subTreeLevel + delta;
    /* The 2 neighbouring nodes of the subtree (the node at the left of the root
    and the node at the right of the last node) is joined. */

    if (root->prevInT)
        (root->prevInT)->nextInT = node->nextInT;
    if (node->nextInT)
        (node->nextInT)->prevInT = root->prevInT;

    return (node);  // the method returns the last node of the subtree
}

template<class N>
void MCFSimplex::PasteSubtree(N *root, N *lastNode, N *previousNode)
{
    N *nextNode = previousNode->nextInT;
    root->prevInT = previousNode;
    previousNode->nextInT = root;
    lastNode->nextInT = nextNode;
    if (nextNode)
        nextNode->prevInT = lastNode;
}

MCFSimplex::arcPType *MCFSimplex::RulePrimalCandidateListPivot(void)
{
    int next = 0;
    int i;
    int minimeValue;
    if (hotListSize < tempCandidateListSize)
        minimeValue = hotListSize;
    else
        minimeValue = tempCandidateListSize;
    // Check if the left arcs in the list continue to violate the dual condition
    for (i = 2; i <= minimeValue; i++)
    {
        arcPType *arc = candP[i].arc;
        int red_cost = ReductCost(arc);

        if (((red_cost < 0) && (arc->ident == AT_LOWER)) ||
            (red_cost > 0 && (arc->ident == AT_UPPER)))
        {
            next++;
            candP[next].arc = arc;
            candP[next].absRC = ABS(red_cost);
        }
    }

    tempCandidateListSize = next;
    int oldGroupPos = groupPos;
    // Search other arcs to fill the list
    do
    {
        arcPType *arc;
        for (arc = arcsP + groupPos; arc < stopArcsP; arc += numGroup)
        {
            if (arc->ident == AT_LOWER)
            {
                int red_cost = ReductCost(arc);
                if (red_cost < 0)
                {
                    tempCandidateListSize++;
                    candP[tempCandidateListSize].arc = arc;
                    candP[tempCandidateListSize].absRC = ABS(red_cost);
                }
            }
            else if (arc->ident == AT_UPPER)
            {
                int red_cost = ReductCost(arc);
                if (red_cost > 0)
                {
                    tempCandidateListSize++;
                    candP[tempCandidateListSize].arc = arc;
                    candP[tempCandidateListSize].absRC = ABS(red_cost);
                }
            }
        }
        groupPos++;
        if (groupPos == numGroup)
            groupPos = 0;

    } while ((tempCandidateListSize < hotListSize) && (groupPos != oldGroupPos));

    if (tempCandidateListSize)
    {
        SortPrimalCandidateList(1, tempCandidateListSize);
        return (candP[1].arc);
    }
    else
        return (NULL);
}

inline void MCFSimplex::InitializePrimalCandidateList(void)
{
    numGroup = ((m - 1) / numCandidateList) + 1;
    groupPos = 0;
    tempCandidateListSize = 0;
}

inline void MCFSimplex::SortPrimalCandidateList(int min, int max)
{
    int left = min;
    int right = max;
    int cut = candP[(left + right) / 2].absRC;
    do
    {
        while (candP[left].absRC > cut)
            left++;
        while (cut > candP[right].absRC)
            right--;

        if (left < right)
        {
            primalCandidType temp = candP[left];
            candP[left] = candP[right];
            candP[right] = temp;
        }

        if (left <= right)
        {
            left++;
            right--;
        }
    } while (left <= right);

    if (min < right)
        SortPrimalCandidateList(min, right);
    if ((left < max) && (left <= hotListSize))
        SortPrimalCandidateList(left, max);
}


template<class N, class RCT>
inline void MCFSimplex::AddPotential(N *r, RCT delta)
{
    int level = r->subTreeLevel;
    N *n = r;
    do
    {
        n->potential = n->potential + delta;
        n = n->nextInT;
    } while ((n) && (n->subTreeLevel > level));
}


template<class N, class A>
inline N *MCFSimplex::Father(N *n, A *a)
{
    if (a == NULL)
        return NULL;

    if (a->tail == n)
        return (a->head);
    else
        return (a->tail);
}


int MCFSimplex::GetCost(void)
{
    int fo = 0;
    arcPType *arco;
    for (arco = arcsP; arco != stopArcsP; arco++)
        if ((arco->ident == BASIC) || (arco->ident == AT_UPPER))
            fo += arco->cost * arco->flow;
    for (arco = dummyArcsP; arco != stopDummyP; arco++)
        if ((arco->ident == BASIC) || (arco->ident == AT_UPPER))
            fo += arco->cost * arco->flow;
    return (fo);
}