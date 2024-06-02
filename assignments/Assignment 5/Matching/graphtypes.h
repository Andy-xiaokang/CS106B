#pragma once

#include <vector>

namespace EdRothberg {
    struct node_entry {
        int degree;
        struct edge_ent *adj_list;
    };
    typedef struct node_entry *Graph;

    struct edge_ent {
        int endpoint;
        int label;
        int label2;
        struct edge_ent *nextedge;
        struct edge_ent *prevedge;
        struct edge_ent *otheredge;
    };
    typedef struct edge_ent *Edge;

    Graph NewGraph(int size);
    int RemoveEdge(Graph graph, Edge edge), NumEdges(Graph g);
    Edge FindEdge(Graph graph, int i, int j);
    void FreeGraph(Graph graph);
    
    /* Adds an edge to a graph. If the edge already exists, this creates a
     * parallel edge.
     */
    void AddEdge (Graph g, int n, int m, int label);

    /* Compute a maximum weighted matching in a general graph. Return value is
     * a list of all the matched pairs, with each match appearing exactly once.
     * Unmatched vertices are omitted.
     *
     * This computes a maximum weighted match, which is not necessarily a
     * perfect match. You can enforce that the matching is perfect by scaling
     * the weights on the edges such that the penalty for not picking an edge
     * exceeds the maximum possible sum of other edges.
     */
    std::vector<std::pair<int, int>> WeightedMatch (Graph gptr);
}
