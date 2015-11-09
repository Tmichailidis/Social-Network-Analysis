#ifndef __Software__Query1__
#define __Software__Query1__

#include "defines.h"
#include "GraphLib.h"

struct InterQueue
{
    int ID,counter;
    Queue * q;
    struct InterQueue * next;
};
typedef struct InterQueue InterQueue;

struct matches{
    int ID;
    double score;
};
typedef struct matches Matches;

struct secondaryInfo
{
    int place;
    Queue * studies, * works;
};
typedef struct secondaryInfo SecInfo;

struct likes
{
    int creatorID,counter;
    struct likes * next;
};
typedef struct likes Likes;

void hpsort(int arr[], int N);
void addInterests(char * file,Graph * g);
void buildGraph(char * placesF, char * studiesF, char * worksF, Graph * secGraph);
Matches* matchSuggestion(Node* n, int k, int h, int x, int limit, Graph* g);
Matches* findMatches(Node* n, int k, int h, int x, int limit, Graph* g,Graph* secg);
Matches * insertionS(Matches * arr,int num);

#endif /* defined(__Software__Query1__) */
