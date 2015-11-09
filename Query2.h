//
//  Query2.h
//  Software
//
//  Created by Theodore Michailidis on 12/14/14.
//  Copyright (c) 2014 Theodore Michailidis. All rights reserved.
//

#ifndef __Software__Query2__
#define __Software__Query2__

#include "defines.h"
#include "GraphLib.h"
#include "Query1.h"
#include "Metrics.h"

typedef Matches Stalkers;
typedef Likes Replies;

Graph* getTopStalkers(int k, int x, int centralityMode, Graph* g, Stalkers* st);
Graph * createPosts(char * file);
void destroyStalkers(Graph *st);
void Metrics(Graph *g);

#endif /* defined(__Software__Query2__) */
