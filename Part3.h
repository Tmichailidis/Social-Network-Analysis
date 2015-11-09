

//
//  Part3.h
//  Software

#ifndef __Software__Part3__
#define __Software__Part3__

#include "defines.h"
#include "GraphLib.h"
#include <pthread.h>
#include <semaphore.h>

typedef struct Job{
    int forumID;
    struct Job * next;
}Job;

typedef struct jobQueue{
    Job * start, * end;
}jobQueue;

typedef struct threadpool{
    int threadsNum,alive,active_thr;
    jobQueue * jobqueue;
    pthread_t * threads;
    pthread_mutex_t * active, *topN_mtx;
    sem_t * sem, * sem2;
}threadPool;

typedef struct args{
    Graph * forumGraph;
    threadPool * thPool;
    int ** topN,N,noe;
}Args;

typedef struct topNGraphs{
    Graph ** graphArray;
    int * graphIDs;
}topNGraphs;

typedef struct args2{
    Graph * g;
    topNGraphs * graphS;
    Args * args;
}Args2;

void * thread_fun(void* arg);
void findTopN(Graph * g, int N);
void destroyForumGraph(Graph * forumGraph);
void DestroyThreadpool_and_args(threadPool * thPool, Args2 * args2);

/*----------- CPM -----------*/

typedef struct Cliques
{
    int * cmember;
    struct Cliques * next;
}Cliques;

typedef struct Communities
{
    int ID;
    void * cliques;
    struct Communities * next;
}Communities;

typedef struct CliqueObj
{
    int * cmember;
    int check;
    Queue * cneighbor;
}CliqueObj;

Communities* cliquePercolationMethod(int k, Graph* g);
Communities* GirvanNewman(double modularity, Graph* g);
void PrintDeleteGirvanNewman(Communities * c);
void PrintDeleteCPM(Communities * com,int k);
void freeGraphStruct(topNGraphs* graphStruct,int N);

#endif /* defined(__Software__Part3__) */
