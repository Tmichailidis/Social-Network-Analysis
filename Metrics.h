//
//  Metrics.h
//  Software
//
//  Created by Theodore Michailidis on 12/14/14.
//  Copyright (c) 2014 Theodore Michailidis. All rights reserved.
//

#ifndef __Software__Metrics__
#define __Software__Metrics__

#include "defines.h"
#include "GraphLib.h"

struct Degree {
    int counter,degree;
    struct Degree * next;
};
typedef struct Degree DegreeList;

struct BetweennessQueue
{
    int dist,ID,path_num;
    struct BetweennessQueue * next;
};
typedef struct BetweennessQueue BeQ;

void degreeDistribution(Graph * g);
int diameter(Graph * g);
double averagePathLength(Graph * g);
int numberOfCCs(Graph * g);
int maxCC(Graph* g);
double density(Graph * g);
double closenessCentrality(Node * n, Graph * g);
double betweennessCentrality(Node * n, Graph * g);

#endif /* defined(__Software__Metrics__) */
