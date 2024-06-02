#include "graphtypes.h"
#include "GraphInternal.h"
#include <stdio.h>
#include <math.h>

namespace EdRothberg {
    void AddEdge (Graph g, int n, int m, int label)
    {	Edge edge1,edge2;

        edge1 = (Edge) malloc(2*sizeof(struct edge_ent));
        edge2 = edge1 + 1;

        edge1->label = label;
        edge1->endpoint = m;
        edge1->otheredge = edge2;
        edge1->prevedge = NULL;
        edge1->nextedge = g[n].adj_list;
        if (edge1->nextedge != NULL)
            edge1->nextedge->prevedge = edge1;
        g[n].adj_list = edge1;
        g[n].degree++;

        edge2->label = label;
        edge2->endpoint = n;
        edge2->otheredge = edge1;
        edge2->prevedge = NULL;
        edge2->nextedge = g[m].adj_list;
        if (edge2->nextedge != NULL)
            edge2->nextedge->prevedge = edge2;
        g[m].adj_list = edge2;
        g[m].degree++;
    }

    Edge FindEdge(Graph graph, int i, int j)
    {	Edge edge;

        edge = graph[i].adj_list;
        while (edge!=NULL && edge->endpoint!=j)
            edge = edge->nextedge;
        if (edge==NULL) return(NULL);
        else return(edge);
    }

    int RemoveEdge(Graph graph, Edge edge)
    {	Edge other;
        int i,j;

        if (edge==NULL) return(0);
        other = edge->otheredge;
        i = other->endpoint;
        j = edge->endpoint;
        graph[i].degree--; graph[j].degree--;
        if (edge->prevedge == NULL) {
            graph[i].adj_list = edge->nextedge;
            if (edge->nextedge != NULL)
                edge->nextedge->prevedge = NULL;
        }
        else if (edge->nextedge == NULL)
            (edge->prevedge)->nextedge = NULL;
        else {
            (edge->nextedge)->prevedge = edge->prevedge;
            (edge->prevedge)->nextedge = edge->nextedge;
        }
        if (other->prevedge == NULL) {
            graph[j].adj_list = other->nextedge;
            if (other->nextedge != NULL)
                other->nextedge->prevedge = NULL;
        }
        else if (other->nextedge == NULL)
            (other->prevedge)->nextedge = NULL;
        else {
            (other->nextedge)->prevedge = other->prevedge;
            (other->prevedge)->nextedge = other->nextedge;
        }
        free((edge < other) ? edge : other);
        return(1);
    }

    int NumEdges(Graph g)
    {	int i,size,edges;

        edges = 0;
        size = Degree(g,0);
        for (i=1; i<=size; i++)
            edges += Degree(g,i);
        edges /= 2;
        return(edges);
    }

    Graph NewGraph(int size)
    {	Graph tmp;
        int i;

        tmp = (Graph) malloc((size+1)*sizeof(struct node_entry));
        for (i=1; i<=size; i++) {
            Degree(tmp,i) = 0;
            FirstEdge(tmp,i) = NULL;
        }
        Degree(tmp,0) = size;
        return(tmp);
    }
    
    /*** New addition to clean up memory allocated for a graph ***/
    
    void FreeGraph(Graph g) {
        /* Wipe the edges. Node count is stored as g[0]'s degree. */
        for (int i = 1; i <= Degree(g, 0); i++) {
            while (g[i].adj_list != nullptr) {
                RemoveEdge(g, g[i].adj_list);
            }
        }
        
        /* Wipe the graph itself. */
        free(g);
    }
}
