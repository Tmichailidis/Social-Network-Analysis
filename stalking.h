#ifndef __STALKING__
#define __STALKING__

#include "defines.h"
#include "metrics.h"
#include "graphLib.h"
#include "matching.h"

typedef Matches Stalkers;
typedef Likes Replies;

Graph* getTopStalkers(int k, int x, int centralityMode, Graph* g, Stalkers* st);
Graph * createPosts(char * file);
void destroyStalkers(Graph *st);
void Metrics(Graph *g);

#endif /* defined(__STALKING__) */
