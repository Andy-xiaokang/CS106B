/* Internal #defines that shouldn't be exported to clients. */

#pragma once

#define Degree(graph,n)    (graph[n].degree)
#define FirstEdge(graph,n) (graph[n].adj_list)

#define EndPoint(e) (e->endpoint)
#define ELabel(e)   (e->label)
#define ELabel2(e)  (e->label2)
#define Other(e)    (e->otheredge)
#define NextEdge(e) (e->nextedge)
