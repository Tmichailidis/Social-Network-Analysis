#ifndef QUERY4_H_
#define QUERY4_H_

#include "defines.h"
#include "GraphLib.h"

Graph* buildTrustGraph(int forum, Graph* g);
Graph * CreateComments(char * file,char * file2, Graph * PostGraph);
void insertLikes(char * file, Graph * postGraph, Graph * trustGraph, Graph * g);
void insertComments(char * file, Graph * CommentsGraph, Graph * postGraph,  Graph * trustGraph, Graph * g);
void addTrust(Graph * trushGraph, Graph * g);
double estimateTrust(Node * a, Node * b, Graph * trustGraph);
void destroyTrustGraph(Graph * trustGraph);

struct LikeCom
{
	int creatorID,likes_count,replies_count;
	struct LikeCom * next;
};

typedef struct LikeCom LikeCom;
#endif /* QUERY4_H_ */
