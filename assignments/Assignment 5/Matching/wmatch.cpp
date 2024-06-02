#include <vector>

/* Written by Edward Rothberg  7/85 */

namespace EdRothberg {
    namespace {
        #define	MAXWT  1000000

        /* the number of the blossom entered by edge e */
        #define BEND(e) (BASE[END[e]])

        /* the blossom matched with v's blossom */
        #define BMATE(v) (BASE[END[MATE[v]]])

        /* the blossom entered by the edge that links v's blossom */
        #define BLINK(v) (BASE[END[LINK[v]]])

        /* the edge e with it's direction reversed */
        #define OPPEDGE(e) (((e - U) % 2 == 0) ? (e - 1) : (e + 1))

        /* the slack of edge e */
        #define SLACK(e) (Y[END[e]] + Y[END[OPPEDGE(e)]] - WEIGHT[e])


        /* Global variables */
        static int *A,*END,*WEIGHT,*NEXTPAIR;
        static int *MATE,*LINK,*BASE,*NEXTVTX,*LASTVTX,*Y,*NEXT_D,*NEXTEDGE;

        static int LAST_D, DELTA;

        static int LASTEDGE[3];

        static int DUMMYVERTEX, DUMMYEDGE;
        static int U, V;

        static int newbase, nextbase, stopscan, pairpoint;
        static int neighbor, nextpoint, newlast;
        static int oldfirst, secondmate;
        static int f, nextedge, nexte, nextu;

        static int v,i,e;
    }
}

#include "graphtypes.h"
#include "GraphInternal.h"
#include <cstdlib>
#include <cstdio>
using namespace std;

namespace EdRothberg {

    /*****************************************************************/
    /* Added: prototypes                                             */
    /*****************************************************************/
    static void LINK_PATH (int e);
    static void MERGE_PAIRS (int v);
    static void SCAN (int x, int del);
    static void INSERT_PAIR ();
    static void UNLINK (int oldbase);
    static void REMATCH (int firstmate, int e);

    static void SetStandard(Graph g);

    static void Initialize();
    static void FreeUp();

    /*****************************************************************/
    /*********************** BEGIN pairs.cpp *************************/
    /*****************************************************************/

    /* Process an edge linking two linked vertices */
    /* Note: global variable v set to the base of one end of the linking edge */

    static void PAIR (int* outcome)
    {   int u, w, temp;

#ifdef DEBUG
        printf("Pair v=%d\n",v);
#endif

        e = NEXTEDGE[v];
        while (SLACK(e) != 2*DELTA)
            e = NEXTPAIR[e];
        w = BEND (e);
        LINK[BMATE (w)] = -e;
        u = BMATE (v);
        while (LINK[u] != -e) {
            LINK[u] = -e;
            if (MATE[w] != DUMMYEDGE) {
                temp = v;
                v = w;
                w = temp; }
            v = BLINK (v);
            u = BMATE (v);
        }
        if (u == DUMMYVERTEX && v != w) {
            *outcome = 1;
            return;
        }
        newlast = v;
        newbase = v;
        oldfirst = NEXTVTX[v];
        LINK_PATH (e);
        LINK_PATH (OPPEDGE (e));
        NEXTVTX[newlast] = oldfirst;
        if (LASTVTX[newbase] == newbase)
            LASTVTX[newbase] = newlast;
        NEXTPAIR[DUMMYEDGE] = DUMMYEDGE;
        MERGE_PAIRS (newbase);
        i = NEXTVTX[newbase];
        do {
            MERGE_PAIRS (i);
            i = NEXTVTX[LASTVTX[i]];
            SCAN (i, 2*DELTA - SLACK(MATE[i]));
            i = NEXTVTX[LASTVTX[i]];
        } while (i != oldfirst);
        *outcome = 0;
        return;
    }


    /* merges a subblossom's pair list into a new blossom's pair list */
    /* v is the base of the previously unlinked subblossom */
    /* Note: global variable newbase set to the base of the new blossom */
    /* 	called with NEXTPAIR[DUMMYEDGE] pointing to the first edge */
    /*		on newbase's pair list */

    static void MERGE_PAIRS (int v)
    {
#ifdef DEBUG
        printf("Merge Pairs v=%d\n",v);
#endif

        NEXT_D[v] = LAST_D;
        pairpoint = DUMMYEDGE;
        f = NEXTEDGE[v];
        while (f != DUMMYEDGE) {
            e = f;
            neighbor = END[e];
            f = NEXTPAIR[f];
            if (BASE[neighbor] != newbase)
                INSERT_PAIR();
        }
    }


    /* links the unlinked vertices in the path P(END[e],newbase) */
    /* Note: global variable newbase is set to the base vertex of the new blossom */
    /*		newlast is set to the last vertex in newbase's current blossom*/

    static void LINK_PATH (int e)
    {   int u;

#ifdef DEBUG
        printf("Link Path e=%d-%d\n", END[OPPEDGE(e)], END[e]);
#endif

        v = BEND (e);
        while (v != newbase) {
            u = BMATE (v);
            LINK[u] = OPPEDGE (e);
            NEXTVTX[newlast] = v;
            NEXTVTX[LASTVTX[v]] = u;
            newlast = LASTVTX[u];
            i = v;
            BASE[i] = newbase;
            i = NEXTVTX[i];
            while (i != DUMMYVERTEX) {
                BASE[i] = newbase;
                i = NEXTVTX[i];
            }
            e = LINK[v];
            v = BEND (e);
        }
    }


    /* Update a blossom's pair list. */
    /* Note: called with global variable e set to the edge to be inserted. */
    /*			neighbor set to the vertex at the end of e */
    /*			pairpoint set to the next pair on the pair list */

    static void INSERT_PAIR ()
    {   int del_e;

#ifdef DEBUG
        printf("Insert Pair e=%d-%d\n",END[OPPEDGE(e)],END[e]);
#endif

        del_e = SLACK(e)/2;
        nextpoint = NEXTPAIR[pairpoint];

        while (END[nextpoint] < neighbor) {
            pairpoint = nextpoint;
            nextpoint = NEXTPAIR[nextpoint];
        }
        if (END[nextpoint] == neighbor) {
            if (del_e >= SLACK (nextpoint)/2)
                return;
            nextpoint = NEXTPAIR[nextpoint];
        }
        NEXTPAIR[pairpoint] = e;
        pairpoint = e;
        NEXTPAIR[e] = nextpoint;
        if (NEXT_D[newbase] > del_e)
            NEXT_D[newbase] = del_e;
    }

    /*****************************************************************/
    /***********************  END  pairs.cpp *************************/
    /*****************************************************************/

    /*****************************************************************/
    /*********************** BEGIN pointer.cpp ***********************/
    /*****************************************************************/
    /* Assign a pointer link to a vertex.  Edge e joins a vertex in blossom */
    /* u to a linked vertex. */

    static void POINTER (int u, int v, int e)
    {   int i, del;

#ifdef DEBUG
        printf("Pointer u,v,e=%d %d %d-%d\n",u,v,END[OPPEDGE(e)],END[e]);
#endif

        LINK[u] = -DUMMYEDGE;
        NEXTVTX[LASTVTX[u]] = DUMMYVERTEX;
        NEXTVTX[LASTVTX[v]] = DUMMYVERTEX;
        
        if (LASTVTX[u] != u) {
            i = MATE[NEXTVTX[u]];
            del = -SLACK(i) / 2;
        }
        else del = LAST_D;

        i = u;
        while (i != DUMMYVERTEX) {
            Y[i] += del;
            NEXT_D[i] += del;
            i = NEXTVTX[i];
        }
        if (LINK[v] < 0) {
            LINK[v] = e;
            NEXTPAIR[DUMMYEDGE] = DUMMYEDGE;
            SCAN (v, DELTA);
            return;
        }
        else {
            LINK[v] = e;
            return;
        }
    }


    /* Scan each vertex in the blossom whose base is x */

    static void SCAN (int x, int del)
    {   int u, del_e;

#ifdef DEBUG
        printf("Scan del=%d x=%d\n",del,x);
#endif

        newbase = BASE[x];
        stopscan = NEXTVTX[LASTVTX[x]];
        while (x != stopscan) {
            Y[x] += del;
            NEXT_D[x] = LAST_D;
            pairpoint = DUMMYEDGE;
            e = A[x];
            while (e != 0) {
                neighbor = END[e];
                u = BASE[neighbor];
                if (LINK[u] < 0) {
                    if (LINK[BMATE (u)] < 0 || LASTVTX[u] != u) {
                        del_e = SLACK (e);
                        if (NEXT_D[neighbor] > del_e) {
                            NEXT_D[neighbor] = del_e;
                            NEXTEDGE[neighbor] = e;
                        }
                    }
                }
                else if (u != newbase) {
                    INSERT_PAIR();
                }
                e = A[e];
            }
            x = NEXTVTX[x];
        }
        NEXTEDGE[newbase] = NEXTPAIR[DUMMYEDGE];
    }


    /*****************************************************************/
    /***********************  END  pointer.cpp ***********************/
    /*****************************************************************/

    /*****************************************************************/
    /*********************** BEGIN readgraph.cpp *********************/
    /*****************************************************************/
    /* set up data structures for weighted match */

    /* to add a new type, add new case in SetUp() and a Set_X() routine */

    static void SetUp (Graph gptr)
    {   int i,allocsize;
        Graph g;

        g = gptr;
        U = Degree(g,0);
        V = NumEdges(g);

        allocsize = (U+2*V+2)*sizeof(int);
        A      = (int *) malloc(allocsize);
        END    = (int *) malloc(allocsize);
        WEIGHT = (int *) malloc(allocsize);
        for (i=0; i<U+2*V+2; i++)
            A[i]=END[i]=WEIGHT[i]=0;

        SetStandard(g);
    }


    /* set up from Type 1 graph. */

    void SetStandard(Graph graph)
    {   int elabel, adj_node, i, j;
        int u, v, currentedge;
        Edge edge;

        currentedge = U+2;
        for (i=1; i<=U; ++i) {
            edge = FirstEdge(graph,i);
            for (j = 1; j <= Degree(graph,i); ++j) {
                adj_node = EndPoint(edge);
                if (i < adj_node) {
                    elabel = ELabel(edge)*2;
                    WEIGHT[currentedge-1] = WEIGHT[currentedge] = 2*elabel;
                    END[currentedge-1] = i;
                    END[currentedge] = adj_node;
                    if (A[i] == 0)
                        A[i] = currentedge;
                    else {
                        u = i;
                        v = A[i];
                        while (v != 0) {
                            if (END[v] > adj_node)
                                break;
                            u = v;
                            v = A[v];
                        }
                        A[u] = currentedge;
                        A[currentedge] = v;
                    }
                    u = adj_node;
                    v = A[u];
                    while (v != 0) {
                        u = v;
                        v = A[v];
                    }
                    A[u] = currentedge - 1;
                    currentedge += 2;
                }
                edge = NextEdge(edge);
            }
        }
    }

    /*****************************************************************/
    /***********************  END  readgraph.cpp *********************/
    /*****************************************************************/

    /*****************************************************************/
    /*********************** BEGIN term.c        *********************/
    /*****************************************************************/
    /* updates numerical bounds for linking paths. */
    /* called with LAST_D set to the bound on DELTA for the next search */

    static void SET_BOUNDS ()

    {   int del;

        for (v=1; v <= U; ++v) {
            if (LINK[v] < 0 || BASE[v] != v) {
                NEXT_D[v] = LAST_D;
                continue;
            }
            LINK[v] = -LINK[v];
            i = v;
            while (i != DUMMYVERTEX) {
                Y[i] -= DELTA;
                i = NEXTVTX[i];
            }
            f = MATE[v];
            if (f != DUMMYEDGE) {
                i = BEND(f);
                del = SLACK(f);
                while (i != DUMMYVERTEX) {
                    Y[i] -= del;
                    i = NEXTVTX[i];
                }
            }
            NEXT_D[v] = LAST_D;
        }
    }


    /* undoes all blossoms to get the final matching */

    static void UNPAIR_ALL ()

    {   int u;

        for (v=1; v <= U; ++v) {
            if (BASE[v] != v || LASTVTX[v] == v)
                continue;
            nextu = v;
            NEXTVTX[LASTVTX[nextu]] = DUMMYVERTEX;
            while (1) {
                u = nextu;
                nextu = NEXTVTX[nextu];
                UNLINK (u);
                if (LASTVTX[u] != u) {
                    f = (LASTEDGE[2] == OPPEDGE(e)) ? LASTEDGE[1] : LASTEDGE[2];
                    NEXTVTX[LASTVTX[BEND(f)]] = u;
                }
                newbase = BMATE (BMATE(u));
                if (newbase != DUMMYVERTEX && newbase != u) {
                    LINK[u] = -DUMMYEDGE;
                    REMATCH (newbase, MATE[u]);
                }
                while (LASTVTX[nextu] == nextu && nextu != DUMMYVERTEX)
                    nextu = NEXTVTX[nextu];
                if (LASTVTX[nextu] == nextu && nextu == DUMMYVERTEX)
                    break;
            }
        }
    }


    /*****************************************************************/
    /***********************  END  term.c          *******************/
    /*****************************************************************/


    /*****************************************************************/
    /*********************** BEGIN unpairs.c          ****************/
    /*****************************************************************/
    /* Expands a blossom.  Fixes up LINK and MATE. */

    static void UNPAIR (int oldbase, int oldmate)
    {   int e, newbase, u;

#ifdef DEBUG
        printf("Unpair oldbase, oldmate=%d %d\n",oldbase, oldmate);
#endif

        UNLINK (oldbase);
        newbase = BMATE (oldmate);
        if (newbase != oldbase) {
            LINK [oldbase] = -DUMMYEDGE;
            REMATCH (newbase, MATE[oldbase]);
            if (f == LASTEDGE[1])
                LINK[secondmate] = -LASTEDGE[2];
            else LINK[secondmate] = -LASTEDGE[1];
        }
        e = LINK[oldmate];
        u = BEND (OPPEDGE (e));
        if (u == newbase) {
            POINTER (newbase, oldmate, e);
            return;
        }
        LINK[BMATE (u)] = -e;
        do {
            e = -LINK[u];
            v = BMATE (u);
            POINTER (u, v, -LINK[v]);
            u = BEND (e);
        } while (u != newbase);
        e = OPPEDGE (e);
        POINTER (newbase, oldmate, e);
    }


    /* changes the matching along an alternating path */
    /* firstmate is the first base vertex on the path */
    /* edge e is the new matched edge for firstmate   */

    static void REMATCH (int firstmate, int e)
    {
#ifdef DEBUG
        printf("Rematch firstmate=%d e=%d-%d\n",firstmate, END[OPPEDGE(e)], END[e]);
#endif

        MATE[firstmate] = e;
        nexte = -LINK[firstmate];
        while (nexte != DUMMYEDGE) {
            e = nexte;
            f = OPPEDGE (e);
            firstmate = BEND (e);
            secondmate = BEND (f);
            nexte = -LINK[firstmate];
            LINK[firstmate] = -MATE[secondmate];
            LINK[secondmate] = -MATE[firstmate];
            MATE[firstmate] = f;
            MATE[secondmate] = e;
        }
    }


    /* unlinks subblossoms in a blossom.  oldbase is the base of the blossom to */
    /* be unlinked. */

    static void UNLINK (int oldbase)
    {   int k, j=1;

#ifdef DEBUG
        printf("Unlink oldbase=%d\n",oldbase);
#endif

        i = NEXTVTX[oldbase];
        newbase = NEXTVTX[oldbase];
        nextbase = NEXTVTX[LASTVTX[newbase]];
        e = LINK[nextbase];
UL2:
	do {
	    nextedge = OPPEDGE (LINK[newbase]);
	    for (k=1; k <= 2; ++k) {
		LINK[newbase] = -LINK[newbase];
		BASE[i] = newbase;
		i = NEXTVTX[i];
		while (i != nextbase) {
		    BASE[i] = newbase;
		    i = NEXTVTX[i];
		}
		newbase = nextbase;
		nextbase = NEXTVTX[LASTVTX[newbase]];
	    }
	} while (LINK[nextbase] == nextedge);
	if (j==1) {
	    LASTEDGE[1] = nextedge;
	    j++;
	    nextedge = OPPEDGE (e);
	    if (LINK[nextbase] == nextedge) {
		goto UL2;
	    }
	}
	LASTEDGE[2] = nextedge;

	if (BASE[LASTVTX[oldbase]] == oldbase)
	    NEXTVTX[oldbase] = newbase;
	else {
	    NEXTVTX[oldbase] = DUMMYVERTEX;
	    LASTVTX[oldbase] = oldbase;
	}
    }


    /*****************************************************************/
    /***********************  END  unpairs.c          ****************/
    /*****************************************************************/

    std::vector<std::pair<int, int>> WeightedMatch (Graph gptr)
    {   int g, j, w, outcome;

        /* set up internal data structure */
        SetUp(gptr);
        Initialize();

        for(;;) {
            /* printf("Augment #%d\n",loop++); */
            DELTA = 0;
            for (v=1; v<=U; ++v)
                if (MATE[v] == DUMMYEDGE)
                    POINTER (DUMMYVERTEX, v, DUMMYEDGE);
            for(;;) {
                i = 1;
                for (j=2; j<=U; ++j)
                    if (NEXT_D[i] > NEXT_D[j])
                        i = j;
                DELTA = NEXT_D[i];
                if (DELTA == LAST_D)
                    goto done;
                v = BASE[i];
                if (LINK[v] >= 0) {
                    PAIR (&outcome);
                    if (outcome == 1)
                        break;
                }
                else {
                    w = BMATE (v);
                    if (LINK[w] < 0) {
                        POINTER (v, w, OPPEDGE(NEXTEDGE[i]));
                    }
                    else UNPAIR (v, w);
                }
            }

            LAST_D -=DELTA;
            SET_BOUNDS();
            g = OPPEDGE(e);
            REMATCH (BEND(e), g);
            REMATCH (BEND(g), e);
        }
        
done:
        SET_BOUNDS();
        UNPAIR_ALL();
        for (i=1; i<=U;++i) {
            MATE[i] = END[MATE[i]];
            if (MATE[i]==DUMMYVERTEX)
                MATE[i]=0;
        }

        /*** NEW: Convert the result to a list of pairs. ***/
        std::vector<std::pair<int, int>> result;
        for (int i = 1; i <= U; i++) {
            /* Include each match once, and skip unmatched vertices. Unmatched
                 * vertices are mated with 0, so this check handles both.
                 */
            if (i < MATE[i]) {
                result.push_back(make_pair(i, MATE[i]));
            }
        }

        FreeUp();
        return result;
    }

    static void Initialize()
    {   int i, allocsize, max_wt= -MAXWT, min_wt=MAXWT;

        DUMMYVERTEX = U+1;
        DUMMYEDGE = U+2*V+1;
        END[DUMMYEDGE] = DUMMYVERTEX;

        for (i=U+2; i<=U+2*V; i+=2) {
            if (WEIGHT[i] > max_wt)
                max_wt = WEIGHT[i];
            if (WEIGHT[i] < min_wt)
                min_wt = WEIGHT[i];
        }
        LAST_D = max_wt/2;

        allocsize = (U+2)*sizeof(int);
        MATE     = (int *) malloc(allocsize);
        LINK     = (int *) malloc(allocsize);
        BASE     = (int *) malloc(allocsize);
        NEXTVTX  = (int *) malloc(allocsize);
        LASTVTX  = (int *) malloc(allocsize);
        Y        = (int *) malloc(allocsize);
        NEXT_D   = (int *) malloc(allocsize);
        NEXTEDGE = (int *) malloc(allocsize);
        allocsize = (U+2*V+2)*sizeof(int);
        NEXTPAIR = (int *) malloc(allocsize);

        for (i = 1; i <= U+1; ++i) {
            MATE[i] = DUMMYEDGE;
            NEXTEDGE[i] = DUMMYEDGE;
            NEXTVTX[i] = 0;
            LINK[i] = -DUMMYEDGE;
            BASE[i] = i;
            LASTVTX[i] = i;
            Y[i] = LAST_D;
            NEXT_D[i] = LAST_D;
        }
    }

    static void FreeUp()
    {
        free(LINK);
        free(BASE);
        free(NEXTVTX);
        free(LASTVTX);
        free(Y);
        free(NEXT_D);
        free(NEXTEDGE);
        free(NEXTPAIR);
        free(A);
        free(END);
        free(WEIGHT);
        
        /* NEW: This is no longer returned to the client. */
        free(MATE);
    }
}
