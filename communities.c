#include <float.h>

#include "trend.h"
#include "metrics.h"
#include "matching.h"
#include "communities.h"

#define Color_Red "\x1b[31m" // Color Start
#define Color_end "\x1b[0m" // To flush out prev settings
#define LOG_RED(X) printf("%s %s %s",Color_Red,X,Color_end)

void PrintDeleteGirvanNewman(Communities * c)     //printCommunitiesofGN
{
    Queue * curQ, * tmpQ;
    Communities * tmpC;
    while(c != NULL)
    {
        printf("Community No%d:{",c->ID);
        curQ = c->cliques;
        printf("%d",curQ->ID);
        curQ = curQ->next;
        free(c->cliques);
        while(curQ != NULL)
        {
            printf(",%d",curQ->ID);
            tmpQ = curQ;
            curQ = curQ->next;
            free(tmpQ);
        }
        printf("}\n");
        tmpC = c;
        c = c->next;
        free(tmpC);
    }
}

void PrintDeleteCPM(Communities * com,int k)
{
    Cliques * tmpClique,* SoC;
    Communities * tmpCom;
    int i;
    while(com != NULL)
    {
        tmpClique = com->cliques;
        printf("Community No%d:{",com->ID);
        while(tmpClique != NULL)
        {
            printf("(%d",tmpClique->cmember[0]);
            for(i=1;i<k;i++)
            {
                printf(",%d",tmpClique->cmember[i]);
            }
            printf(")");
            SoC = tmpClique;
            tmpClique = tmpClique->next;
            free(SoC);
        }
        printf("}\n");
        tmpCom = com;
        com = com->next;
        free(tmpCom);
    }
}

void * assign_job(void * arg)
{
    int count;
    Node * n;
    Queue * q;
    Job * tmp;
    Args * args = arg;
    threadPool * thrPool = args->thPool;
    Graph * FGraph = args->forumGraph;
    while(thrPool->alive)
    {
        if (thrPool->jobqueue->start == NULL)
        {
            pthread_mutex_lock(thrPool->active);
            thrPool->active_thr--;
            if (thrPool->active_thr == 0 && thrPool->alive == FGraph->nodes_sum+1)  //last active thread
                sem_post(thrPool->sem2); 
            pthread_mutex_unlock(thrPool->active);
            sem_wait(thrPool->sem);           //blocking thread till activation
            pthread_mutex_lock(thrPool->active);
            thrPool->active_thr++;
            pthread_mutex_unlock(thrPool->active);
        }
        pthread_mutex_lock(thrPool->active);
        if (thrPool->alive && thrPool->jobqueue->start != NULL)
        {
            thrPool->alive++;
            tmp = thrPool->jobqueue->start;   //assigning job 
            thrPool->jobqueue->start = thrPool->jobqueue->start->next;
            pthread_mutex_unlock(thrPool->active);
            n = lookupNode(tmp->forumID, FGraph);
            q = n->obj;
            count = 0;
            while(q != NULL)  //count forum member
            {
                count++;
                q = q->next;
            }
            pthread_mutex_lock(thrPool->topN_mtx);
            if(args->noe<args->N) //replace forum with minimum members
            {
                
                args->topN[0][args->noe] = tmp->forumID;
                args->topN[1][args->noe] = count;
                args->noe++;
                if(args->noe==args->N)
                    insertionT(args->topN, args->N);
            }
            else if(count > args->topN[1][0])
            {
                args->topN[0][0] = tmp->forumID;
                args->topN[1][0] = count;
                insertionT(args->topN, args->N);
            }
            pthread_mutex_unlock(thrPool->topN_mtx);
            free(tmp);
        }
        else
            pthread_mutex_unlock(thrPool->active);
    }
    return NULL;
}


threadPool * threadPool_init(Args * argSet,int threadsNum)
{
    threadPool * ThreadPool = malloc(sizeof(threadPool));
    ThreadPool->alive = 1;
    ThreadPool->active_thr = ThreadPool->threadsNum = threadsNum;
    ThreadPool->jobqueue = malloc(sizeof(jobQueue));
    ThreadPool->jobqueue->start = NULL;
    ThreadPool->jobqueue->end = NULL;
    ThreadPool->threads = malloc(threadsNum*sizeof(pthread_t));
    ThreadPool->active = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(ThreadPool->active, NULL);
    ThreadPool->topN_mtx = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(ThreadPool->topN_mtx, NULL);
    ThreadPool->sem = malloc(sizeof(sem_t));
    sem_init(ThreadPool->sem, 0, 0);
    ThreadPool->sem2 = malloc(sizeof(sem_t));
    sem_init(ThreadPool->sem2, 0, 0);
    argSet->thPool = ThreadPool;
    int i;
    for(i=0; i<threadsNum; i++)
        pthread_create(&ThreadPool->threads[i], NULL,assign_job, argSet);
    return ThreadPool;
}

void add_job(int fID, threadPool * threadPool)
{
    Job * job = malloc(sizeof(Job));
    job->forumID = fID;
    job->next = NULL;
    if (threadPool->jobqueue->start == NULL)
    {
        threadPool->jobqueue->start = threadPool->jobqueue->end = job;
        sem_post(threadPool->sem);  //activate thread
    }
    else
    {
        threadPool->jobqueue->end->next = job;
        threadPool->jobqueue->end = threadPool->jobqueue->end->next;
        if(threadPool->active_thr < threadPool->threadsNum)
            sem_post(threadPool->sem);
    }
    
}

void preproccess(Graph * forumGraph) //forum graph creation
{
    FILE* fp = fopen("forum_hasMember_person.csv", "r");
    if (fp == NULL)
    {
        perror ("Error opening file");
        exit(-1);
    }
    char buffer[200];
    Queue * q;
    int fID;
    Node * n;
    fgets(buffer,200,fp);
    while(fgets(buffer,200,fp)!= NULL) 
    {
        fID = atoi(strtok(buffer,"|"));
        q = malloc(sizeof(Queue));
        q->ID = atoi(strtok(NULL,"|"));
        if ((n = lookupNode(fID,forumGraph))== NULL)
        {
            n = malloc(sizeof(Node));
            n->ID = fID;
            q->next = NULL;
            n->obj = (void *) q;
            insertNode(n,forumGraph);
            continue;
        }
        q->next = n->obj;
        n->obj = q;
    }
    fclose(fp);
}

void * createForumGraphs(void * arg)
{
    Args2 * args2 = arg;
    Node * Fnode,* Mnode, * tmpNode;
    Queue * q;
    List * l;
    Person * p;
    int index,x,i = args2->args->N,j;
    topNGraphs * graphStruct = args2->graphS;
    threadPool * thrPool = args2->args->thPool;
    
    while(thrPool->alive)
    {
        pthread_mutex_lock(thrPool->topN_mtx);
        if(args2->args->noe < args2->args->N)
        {
            i = args2->args->noe;
            args2->args->noe++;
            pthread_mutex_unlock(thrPool->topN_mtx);
            graphStruct->graphArray[i] = createGraph(M,C);   //graph creation for topN forums
            Fnode = lookupNode(args2->args->topN[0][i], args2->args->forumGraph);
            q = Fnode->obj;
            while( q!= NULL ) //insert members
            {
                Mnode = createPerson(q->ID, NULL);
                insertNode(Mnode, graphStruct->graphArray[i]);
                q = q->next;
            }
        }
        else
        {
            pthread_mutex_unlock(thrPool->topN_mtx);
            pthread_mutex_lock(thrPool->active);
            thrPool->active_thr--;
            pthread_mutex_unlock(thrPool->active);
            if(thrPool->active_thr == 0) //last thread activates main thread
                sem_post(thrPool->sem2);
            sem_wait(thrPool->sem);
            pthread_mutex_lock(thrPool->active);
            thrPool->active_thr++;
            pthread_mutex_unlock(thrPool->active);
        }
        Edge * e;
        if(i < args2->args->N)
        {
            for (x = 0;x< graphStruct->graphArray[i]->size;x++)
            {
                for(j = 0;j<graphStruct->graphArray[i]->position[x];j++)
                {
                    Mnode = lookupNode(graphStruct->graphArray[i]->table[x][j]->ID,args2->g); //search current node in graph g
                    p = Mnode->obj;
                    if ((l = p->list)==NULL) //checking for neighbors
                        continue;
                    while(l != NULL)
                    {
                        if((tmpNode = lookupNode(l->neighbor->ID,graphStruct->graphArray[i]))==NULL) //checking if neighbor is in the same forum
                        {
                            l = l->next;
                            continue;
                        }
                        index = l->neighbor->ID %graphStruct->graphArray[i]->hash;
                        if (graphStruct->graphArray[i]->h_pointer > index) //If it is before the hash_pointer
                            index = l->neighbor->ID%(2*graphStruct->graphArray[i]->hash);
                        if (index < x || (index == x && l->neighbor->ID < graphStruct->graphArray[i]->table[x][j]->ID))
                        {
                            l = l->next;
                            continue;
                        }
                        /* create edge between the 2 nodes */
                        pthread_mutex_lock(thrPool->topN_mtx);
                        e = createEdge(0,l->neighbor->ID,NULL);
                        insertEdge(graphStruct->graphArray[i]->table[x][j]->ID,e,graphStruct->graphArray[i]);
                        e = createEdge(0,graphStruct->graphArray[i]->table[x][j]->ID,NULL);
                        insertEdge(l->neighbor->ID,e,graphStruct->graphArray[i]);
                        pthread_mutex_unlock(thrPool->topN_mtx);
                        l = l->next;
                    }
                }
            }
            i = args2->args->N;
        }
    }
    return NULL;
}

void freeGraphStruct(topNGraphs * graphS, int N)
{
    int i,j,k;
    Person * p;
    List * tmpL;
    for(k=0; k<N; k++) //for every topN forum graph
    {
        for(i=0; i < graphS->graphArray[k]->size; i++)
        {
            for(j=0; j < graphS->graphArray[k]->position[i]; j++)
            {
                p = graphS->graphArray[k]->table[i][j]->obj;
                while(p->list != NULL)
                {
                    tmpL = p->list;
                    if(tmpL->neighbor->eProp != NULL)
                    {
                        free(tmpL->neighbor->eProp->prop);
                        free(tmpL->neighbor->eProp); //deallocation of edges
                    }
                    free(tmpL->neighbor);
                    p->list = p->list->next;
                    free(tmpL);
                }
                free(p->nProp->prop[0]);
                free(p->nProp->prop);
                free(p->nProp);
                free(p->pair);
                free(p);
                free(graphS->graphArray[k]->table[i][j]); //Node deallocation
            }
            free(graphS->graphArray[k]->table[i]); //Bucket deallocation
        }
        free(graphS->graphArray[k]->table);
        free(graphS->graphArray[k]->cell_size);
        free(graphS->graphArray[k]->position);
        free(graphS->graphArray[k]);
    }
    free(graphS->graphArray);
    free(graphS->graphIDs);
    free(graphS);
}

void findTopN(Graph * g, int N)
{
    Graph * forumGraph = createGraph(M,C);
    preproccess(forumGraph);
    Args *args = malloc(sizeof(Args));  //pthread_create arguments
    args->N = N;
    args->forumGraph = forumGraph;
    args->topN = malloc(2*sizeof(int *));
    args->noe = 0;
    int i,j;
    args->topN[0] = malloc(N*sizeof(int));
    args->topN[1] = malloc(N*sizeof(int));
    for(i=0; i<N; i++)
    {
        args->topN[0][i] = 0;
        args->topN[1][i] = 0;
    }
    int number_of_threads = 10;
    threadPool * ThreadPool = threadPool_init(args,number_of_threads);
    for(i=0;i<forumGraph->size;i++)
    {
        for(j=0;j<forumGraph->position[i];j++)
        {
            pthread_mutex_lock(ThreadPool->active);
            add_job(forumGraph->table[i][j]->ID,ThreadPool);
            pthread_mutex_unlock(ThreadPool->active);
        }
    }
    sem_wait(ThreadPool->sem2);  //waiting for threads
    pthread_mutex_lock(ThreadPool->active);
    ThreadPool->alive = 0;
    pthread_mutex_unlock(ThreadPool->active);
    for(i=0; i<number_of_threads; i++)
        if (sem_post(ThreadPool->sem))
            perror("Could not bypass sem_wait()\n");
    for(i=0; i<ThreadPool->threadsNum; i++)
        pthread_join(ThreadPool->threads[i],NULL);
    printf("~~~~~~~~~~~topN forums:~~~~~~~~~~~~~~~~~~~\n");
    for(i=0; i<N; i++)
        printf("forum with id %d has %d members\n",args->topN[0][i], args->topN[1][i]);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    
    topNGraphs * graphStruct = malloc(sizeof(topNGraphs));
    graphStruct->graphIDs = args->topN[0];  //topN forums ids
    graphStruct->graphArray = malloc(N*sizeof(Graph *));  //topN forums graph array
    Args2 * args2 = malloc(sizeof(Args2));
    args2->graphS = graphStruct;
    args2->g = g;
    args2->args = args;
    args2->args->forumGraph = forumGraph;
    args2->args->N = N;
    args2->args->noe = 0;
    ThreadPool->alive = 1;
    sem_init(ThreadPool->sem, 0,0);   //initialization
    sem_init(ThreadPool->sem2, 0,0);
    for(i=0; i< ThreadPool->threadsNum; i++)
        pthread_create(&ThreadPool->threads[i], NULL, createForumGraphs, args2);
    sem_wait(ThreadPool->sem2);
    pthread_mutex_lock(ThreadPool->active);
    ThreadPool->alive = 0;
    pthread_mutex_unlock(ThreadPool->active);
    for(i=0; i<number_of_threads; i++)
        if (sem_post(ThreadPool->sem)) 
            perror("Could not bypass sem_wait()\n");
    for(i=0; i<ThreadPool->threadsNum; i++)
        pthread_join(ThreadPool->threads[i],NULL);
    DestroyThreadpool_and_args(ThreadPool,args2); //free resources -- threadpool & arguments
    destroyForumGraph(forumGraph);
    Person * p;
    List * l;
    int k,x,* neighbors;
    for(k = 0;k < N;k++)
    {
        for(i = 0;i < graphStruct->graphArray[k]->size;i++) //create neighbors array
        {
            for(j = 0;j < graphStruct->graphArray[k]->position[i];j++)
            {
                p = graphStruct->graphArray[k]->table[i][j]->obj;
                neighbors = malloc(p->edges*sizeof(int));
                l = p->list;
                x = 0;
                while(l != NULL)
                {
                    neighbors[x++] = l->neighbor->ID;
                    l = l->next;
                }
                p->nProp = malloc(sizeof(Properties));
                p->nProp->prop = malloc(2*sizeof(void *));
                p->nProp->prop[0] = neighbors;
                p->nProp->prop[1] = 0;
            }
        }
    }
    Communities *cGN;
    for (i=0;i<k;i++)
    {
        LOG_RED("\n\nForum with ID = ");
        printf("%d\n",graphStruct->graphIDs[i]);
        cliquePercolationMethod(3,graphStruct->graphArray[i]);
        cGN = GirvanNewman(DBL_MAX,graphStruct->graphArray[i]);
        PrintDeleteGirvanNewman(cGN);
    }
    freeGraphStruct(graphStruct, N);
}

void destroyForumGraph(Graph * forumGraph)
{
    int i,j;
    Queue * q, * tmp;
    for(i=0; i<forumGraph->size; i++)
    {
        for(j=0; j<forumGraph->position[i]; j++)
        {
            q = forumGraph->table[i][j]->obj;
            while(q!= NULL)
            {
                tmp = q;
                q = q->next;
                free(tmp);
            }
            free(forumGraph->table[i][j]);
        }
        free(forumGraph->table[i]);
    }
    free(forumGraph->table);
    free(forumGraph->position);
    free(forumGraph->cell_size);
    free(forumGraph);
}

void DestroyThreadpool_and_args(threadPool * thPool, Args2 * args2)
{
    free(args2->args->topN[1]);
    free(args2->args->topN);
    free(args2->args);
    free(args2);
    pthread_mutex_destroy(thPool->topN_mtx);
    pthread_mutex_destroy(thPool->active);
    sem_destroy(thPool->sem);
    sem_destroy(thPool->sem2);
    free(thPool->sem);
    free(thPool->sem2);
    free(thPool->topN_mtx);
    free(thPool->active);
    free(thPool->threads);
    free(thPool->jobqueue);
    free(thPool);
}

void destroyCliqueGraph(Graph * cliqueG)
{
    int i,j;
    CliqueObj * tmpObj;
    Queue * q;
    for(i=0; i < cliqueG->size; i++)
    {
        for(j=0; j < cliqueG->position[i]; j++)
        {
            tmpObj = cliqueG->table[i][j]->obj;
            free(tmpObj->cmember);     //clique member array deletion
            while(tmpObj->cneighbor != NULL) 
            {
                q = tmpObj->cneighbor;
                tmpObj->cneighbor = tmpObj->cneighbor->next;
                free(q);
            }
            free(tmpObj);
            free(cliqueG->table[i][j]);
        }
        free(cliqueG->table[i]);
    }
    free(cliqueG->table);
    free(cliqueG->position);
    free(cliqueG->cell_size);
    free(cliqueG);
}

Communities* cliquePercolationMethod(int k, Graph * g)
{
    int i,j,x=0,y=0,hash_index, * neighbors, * index,tmpIndex;
    Person * p, *tmpPerson;
    Node * tmpNode;
    Cliques * SoC = NULL,*tmpClique; //SoC: start of cliques
    printf("\nCliquePercolationMethod starts!\n");
    p = g->table[0][0]->obj;
    index = malloc((k-1)*sizeof(int));
    for(i = 0;i < g->size;i++)
    {
        for(j = 0;j < g->position[i];j++)
        {
            p = g->table[i][j]->obj;
            if (p->edges < k-1) //if number_of_neighbors <  k-1, continue
                continue;
            neighbors = p->nProp->prop[0];
            for(x = 0;x < k-1; x++)
                index[x] = x;
            while(1) //while the first index has not taken the last value
            {
                for(x = 0;x < k-2;x++) //checking for clique
                {
                    hash_index = neighbors[index[x]]%g->hash;
                    if (g->h_pointer > hash_index) //Before the hash_pointer
                        hash_index = neighbors[index[x]]%(2*g->hash);
                    if ((hash_index < i) || (hash_index == i && g->table[i][j]->ID > neighbors[index[x]]))
                        break;
                    for(y = x+1;y < k-1;y++)
                    {
                        hash_index = neighbors[index[y]]%g->hash;
                        if (g->h_pointer > hash_index) 
                            hash_index = neighbors[index[y]]%(2*g->hash);
                        if ((hash_index < i) || (hash_index == i && g->table[i][j]->ID > neighbors[index[y]]))
                            break;
                        if ((tmpNode = lookupNode(neighbors[index[x]],g))==NULL)
                            exit(-1);
                        tmpPerson = tmpNode->obj;
                        if ((binary_search(tmpPerson->nProp->prop[0],tmpPerson->edges,neighbors[index[y]])) == -1)
                            break;
                    }
                    if (y != k-1) //last index has not taken the last value
                        break;
                }
                if (x == k-2 && y == k-1) //clique creation
                {
                    tmpClique = malloc(sizeof(Cliques));
                    tmpClique->cmember = malloc(k*sizeof(int));
                    tmpClique->cmember[0] = g->table[i][j]->ID;
                    for(y = 1;y < k;y++)
                        tmpClique->cmember[y] = neighbors[index[y-1]];
                    tmpClique->next = SoC;
                    SoC = tmpClique;
                }
                tmpIndex = k-2;
                if (index[0] == p->edges-k+1)
                    break;
                while(1) //index change
                {
                    if (index[tmpIndex] == p->edges-k+1+tmpIndex)
                        tmpIndex--;
                    else
                    {
                        y = ++index[tmpIndex];
                        for(x = tmpIndex+1; x < k-1; x++)
                            index[x] = ++y;
                        break;
                    }
                }
            }
        }
    }
    free(index);

    Graph * cliqueG = createGraph(M,C);
    CliqueObj * tmpObj, *tmpObj2;
    int id = 1;
    while(SoC != NULL)  //create graph with every clique
    {
        tmpNode = malloc(sizeof(Node));
        tmpNode->ID = id++;
        tmpObj = malloc(sizeof(CliqueObj));
        hpsort(SoC->cmember,k);
        tmpObj->cmember = SoC->cmember;
        tmpObj->check = -1;
        tmpObj->cneighbor = NULL;
        tmpNode->obj = tmpObj;
        insertNode(tmpNode,cliqueG);
        tmpClique = SoC;
        SoC = SoC->next;
        free(tmpClique); 
    }
    
    int z,w,e;
    Queue * tmpQ;
    int * tmpcm,*tmpcm2;
    for(i=0;i<cliqueG->size;i++)
    {
        for(j=0;j<cliqueG->position[i];j++)
        {
            tmpObj = cliqueG->table[i][j]->obj;
            tmpcm = tmpObj->cmember;
            for(x=i;x<cliqueG->size;x++)
            {
                if(x == i)
                    tmpIndex = j+1;
                else
                    tmpIndex = 0;
                for(y=tmpIndex;y<cliqueG->position[x];y++)
                {
                    e = w = z = 0;
                    tmpObj2 = cliqueG->table[x][y]->obj;
                    tmpcm2 = tmpObj2->cmember;
                    while(w < k && z < k)
                    {
                        if (tmpcm[w] != tmpcm2[z]) //if elements are different
                        {
                            e++;
                            if (e == 3)
                                break;
                            else if(tmpcm[w] < tmpcm2[z])
                                w++;
                            else
                                z++;
                        }
                        else
                        {
                            w++;
                            z++;
                        }
                    }
                    if (e == 3)
                        continue;
                    if (w == z || (e == 1))  //2 cliques with k-1 common members are neighbors
                    {
                        tmpQ = malloc(sizeof(Queue));
                        tmpQ->ID = cliqueG->table[i][j]->ID;
                        tmpQ->next = tmpObj2->cneighbor;
                        tmpObj2->cneighbor = tmpQ;
                        tmpQ = malloc(sizeof(Queue));
                        tmpQ->ID = cliqueG->table[x][y]->ID;
                        tmpQ->next = tmpObj->cneighbor;
                        tmpObj->cneighbor = tmpQ;
                    }
                }
            }
        }
    }

    Queue *end,*q, *tmpq;
    Communities  * com = NULL, *tmpCom = NULL;
    id = 1;
    for(i=0;i<cliqueG->size;i++)  //communities creation
    {
        for(j=0;j<cliqueG->position[i];j++)
        {
            tmpCom = NULL;
            tmpObj = cliqueG->table[i][j]->obj;
            if (tmpObj->check != -1)
                continue;
            tmpObj->check = 1;
            tmpCom = malloc(sizeof(Communities));
            tmpClique = malloc(sizeof(Cliques));
            tmpClique->cmember = tmpObj->cmember;
            tmpClique->next = NULL;
            tmpCom->cliques = tmpClique;
            tmpCom->ID = id++;
            tmpQ = tmpObj->cneighbor;
            if (tmpQ == NULL)  //sole community without neighbors
            {
                tmpCom->next = com;
                com = tmpCom;
                continue;
            }
            /*else community creation */
            tmpNode = lookupNode(tmpQ->ID,cliqueG);
            tmpObj = tmpNode->obj;
            tmpObj->check = 1;
            q = malloc(sizeof(Queue));
            q->ID = tmpQ->ID;
            q->next = NULL;
            end = q;
            tmpQ = tmpQ->next;
            while(tmpQ != NULL) //cliques neighbor list and marking
            {
                tmpNode = lookupNode(tmpQ->ID,cliqueG);
                tmpObj = tmpNode->obj;
                if (tmpObj->check != -1) //marked already
                {
                    tmpQ = tmpQ->next;
                    continue;
                }
                tmpObj->check = 1; //mark
                end->next = malloc(sizeof(Queue));
                end->next->ID = tmpQ->ID;
                end->next->next = NULL;
                end = end->next;
                tmpQ = tmpQ->next;
            }
            while(q != NULL)  //insert clique in the community
            {
                tmpNode = lookupNode(q->ID,cliqueG);
                tmpObj = tmpNode->obj;
                tmpClique = malloc(sizeof(Cliques));
                tmpClique->cmember = tmpObj->cmember;
                tmpClique->next = tmpCom->cliques;
                tmpCom->cliques = tmpClique;
                tmpQ = tmpObj->cneighbor;
                while(tmpQ != NULL)  
                {
                    tmpNode = lookupNode(tmpQ->ID,cliqueG);
                    tmpObj = tmpNode->obj;
                    if (tmpObj->check != -1)
                    {
                        tmpQ = tmpQ->next;
                        continue;
                    }
                    tmpObj->check = 1;
                    end->next = malloc(sizeof(Queue));
                    end->next->ID = tmpQ->ID;
                    end->next->next = NULL;
                    end = end->next;
                    tmpQ = tmpQ->next;
                }
                tmpq = q;
                q = q->next;
                free(tmpq);
            }
            if (tmpCom != NULL) // insert community in the list
            {
                tmpCom->next = com;
                com = tmpCom;
            }
        }
    }
    for (i=0;i<g->size;i++)
    {
        for(j=0;j<g->position[i];j++)
        {
            tmpPerson = g->table[i][j]->obj;
            tmpPerson->distN = -1;
        }
    }
    PrintDeleteCPM(com,k);
    destroyCliqueGraph(cliqueG);
    return NULL;
}

int isPrevious(int currentID,int candidateID,int hash,int h_pointer,int level)
{
    int bucket = candidateID%hash;
    if (h_pointer > bucket) ///Before hash_pointer
        bucket = candidateID%(2*hash);
    if ((bucket < level) || (bucket == level && currentID > candidateID))
        return 1;
    return 0;
}

void edgeBetweeness(Graph * g,int *st,int *e)
{
    int i,j,cur_bfs = -1,s_node=-1,e_node=-1,x,y,z,w,tmp2,std,sid,itd,md,efound,start = 0,end = 0;
    Node * tmpNode,*curNode;
    List * mainList, *secList;
    Person * mainP, *tmpP, *curP;
    Queue * curQ = NULL ,*endQ = NULL ,*tmpQ = NULL;
    float edgeB = 0.0,max = 0.0;
    for(i = 0; i < g->size;i++)
    {
        for(j = 0;j < g->position[i];j++)
        {
            mainP = g->table[i][j]->obj;
            mainList = mainP->list;
            while(mainList != NULL)
            {
                edgeB = 0.0;
                if (isPrevious(g->table[i][j]->ID,mainList->neighbor->ID,g->hash,g->h_pointer,i))
                {
                    mainList = mainList->next;
                    continue;
                }
                for(x = 0;x < g->size;x ++)
                {
                    for (y = 0;y < g->position[x];y++)
                    {
                        for (z = x;z < g->size;z++)
                        {
                            if (x == z)
                                tmp2 = y + 1;
                            else
                                tmp2 = 0;
                            for (w = tmp2; w < g->position[z];w++)
                            {
                                //if start node and end node share an edge
                                if ((g->table[i][j]->ID == g->table[x][y]->ID && mainList->neighbor->ID == g->table[z][w]->ID) || (mainList->neighbor->ID == g->table[x][y]->ID && g->table[i][j]->ID == g->table[z][w]->ID))
                                {
                                    edgeB += 1.0;
                                    continue;
                                }
                                std = reachNode1(g->table[x][y]->ID,g->table[z][w]->ID,g);
                                md = reachNode1(g->table[x][y]->ID,g->table[i][j]->ID,g);
                                sid = reachNode1(g->table[x][y]->ID,mainList->neighbor->ID,g);
                                if (md <= sid)
                                    itd = reachNode1(mainList->neighbor->ID,g->table[z][w]->ID,g);
                                else
                                {
                                    itd = reachNode1(g->table[i][j]->ID,g->table[z][w]->ID,g);
                                    md = sid;
                                }
                                if ((md >= std) || (std-md != itd + 1))
                                    continue;
                                s_node = -1;
                                e_node = -1;
                                efound = 0;
                                if(g->table[i][j]->ID == g->table[x][y]->ID)
                                {
                                    s_node = g->table[i][j]->ID;
                                    e_node = mainList->neighbor->ID;
                                }
                                if(mainList->neighbor->ID == g->table[x][y]->ID)
                                {
                                    e_node = g->table[i][j]->ID;
                                    s_node = mainList->neighbor->ID;
                                }
                                curP = g->table[x][y]->obj;
                                curP->nProp->prop[1] = 0;
                                secList = curP->list;
                                curP->distN = ++cur_bfs;
                                tmpNode = lookupNode(secList->neighbor->ID,g);
                                tmpQ = malloc(sizeof(Queue));
                                tmpQ->ID = secList->neighbor->ID;
                                tmpQ->next = NULL;
                                curQ = endQ = tmpQ;
                                tmpP = tmpNode->obj;
                                tmpP->distN = cur_bfs;
                                tmpP->nProp->prop[1] = (void *) 1;
                                tmpP->pair->distance = 1;
                                if(s_node != -1) //found first node
                                {
                                    if(e_node == secList->neighbor->ID)
                                    {
                                        tmpP->pair->ID = 1;
                                        efound = 1; //found second node
                                    }
                                    else
                                        tmpP->pair->ID = 0;
                                    secList = secList->next;
                                    while(efound == 0) //searching for the second node
                                    {
                                        tmpNode = lookupNode(secList->neighbor->ID,g);
                                        tmpQ = malloc(sizeof(Queue));
                                        tmpQ->ID = secList->neighbor->ID;
                                        tmpQ->next = NULL;
                                        endQ->next = tmpQ;
                                        endQ = tmpQ;
                                        tmpP = tmpNode->obj;
                                        tmpP->distN = cur_bfs;
                                        tmpP->pair->distance = 1;
                                        tmpP->nProp->prop[1] =(void *) 1;
                                        if(e_node == secList->neighbor->ID) //found first node
                                        {
                                            tmpP->pair->ID = 1;
                                            efound = 1; //found second one
                                        }
                                        else
                                            tmpP->pair->ID = 0;
                                        secList = secList->next;
                                    }
                                }
                                else
                                {
                                    tmpP->pair->ID = 0;
                                    secList = secList->next;
                                }
                                while(secList != NULL)
                                {
                                    tmpNode = lookupNode(secList->neighbor->ID,g);
                                    tmpQ = malloc(sizeof(Queue));
                                    tmpQ->ID = secList->neighbor->ID;
                                    tmpQ->next = NULL;
                                    endQ->next = tmpQ;
                                    endQ = tmpQ;
                                    tmpP = tmpNode->obj;
                                    tmpP->distN = cur_bfs;
                                    tmpP->pair->distance = 1;
                                    tmpP->pair->ID = 0;
                                    tmpP->nProp->prop[1] = (void *) 1;
                                    secList = secList->next;
                                }
                                while(efound == 0) //if edge is not marked
                                {
                                    curNode = lookupNode(curQ->ID,g);
                                    curP = curNode->obj;
                                    secList = curP->list;
                                    if(g->table[i][j]->ID == curNode->ID)
                                    {
                                        s_node = g->table[i][j]->ID;
                                        e_node = mainList->neighbor->ID;
                                    }
                                    if(mainList->neighbor->ID == curNode->ID)
                                    {
                                        e_node = g->table[i][j]->ID;
                                        s_node = mainList->neighbor->ID;
                                    }
                                    if(s_node != -1) // if the first node has been found
                                    {
                                        while(efound == 0 && secList != NULL) //second node searching
                                        {
                                            tmpNode = lookupNode(secList->neighbor->ID,g);
                                            tmpP = tmpNode->obj;
                                            if (tmpP->distN == cur_bfs) //same bfs
                                            {
                                                if (tmpP->nProp->prop[1] == curP->nProp->prop[1]+1) //if we have reached previously this depth
                                                {
                                                    tmpP->pair->ID += curP->pair->ID;
                                                    tmpP->pair->distance += curP->pair->distance;
                                                    if(e_node == secList->neighbor->ID) //second node 
                                                    {
                                                        tmpP->pair->ID = curP->pair->distance;
                                                        efound = 1; //found the second node
                                                    }
                                                    else
                                                        tmpP->pair->ID = 0;
                                                }
                                                secList = secList->next;
                                                continue;
                                            }
                                            tmpQ = malloc(sizeof(Queue));
                                            tmpQ->ID = secList->neighbor->ID;
                                            tmpQ->next = NULL;
                                            endQ->next = tmpQ;
                                            endQ = tmpQ;
                                            tmpP->distN = cur_bfs;
                                            tmpP->pair->distance = curP->pair->distance;
                                            tmpP->nProp->prop[1] = curP->nProp->prop[1]+1;
                                            if(e_node == secList->neighbor->ID) //second node 
                                            {
                                                tmpP->pair->ID = curP->pair->distance;
                                                efound = 1; //found the second node
                                            }
                                            else
                                                tmpP->pair->ID = 0;
                                            secList = secList->next;
                                        }
                                    }
                                    while(secList != NULL)
                                    {
                                        tmpNode = lookupNode(secList->neighbor->ID,g);
                                        tmpP = tmpNode->obj;
                                        if (tmpP->distN == cur_bfs)
                                        {
                                            if (tmpP->nProp->prop[1] == curP->nProp->prop[1]+1)
                                            {
                                                tmpP->pair->ID += curP->pair->ID;
                                                tmpP->pair->distance += curP->pair->distance;
                                            }
                                            secList = secList->next;
                                            continue;
                                        }
                                        tmpQ = malloc(sizeof(Queue));
                                        tmpQ->ID = secList->neighbor->ID;
                                        tmpQ->next = NULL;
                                        endQ->next = tmpQ;
                                        endQ = tmpQ;
                                        tmpP->distN = cur_bfs;
                                        tmpP->pair->distance = curP->pair->distance;
                                        tmpP->pair->ID = curP->pair->ID;
                                        tmpP->nProp->prop[1] = curP->nProp->prop[1]+1;
                                        secList = secList->next;
                                    }
                                    curP->pair->ID = -1;
                                    curP->pair->distance = -1;
                                    tmpQ = curQ;
                                    curQ = curQ->next;
                                    free(tmpQ);
                                }
                                //found the edge
                                while(curQ->ID != g->table[z][w]->ID)
                                {
                                    
                                    curNode = lookupNode(curQ->ID,g);
                                    curP = curNode->obj;
                                    secList = curP->list;
                                    while(secList != NULL)
                                    {
                                        tmpNode = lookupNode(secList->neighbor->ID,g);
                                        tmpP = tmpNode->obj;
                                        if (tmpP->distN == cur_bfs) 
                                        {
                                            if (tmpP->nProp->prop[1] == curP->nProp->prop[1]+1) 
                                            {
                                                tmpP->pair->ID += curP->pair->ID;
                                                tmpP->pair->distance += curP->pair->distance;
                                            }
                                            secList = secList->next;
                                            continue;
                                        }
                                        tmpQ = malloc(sizeof(Queue));
                                        tmpQ->ID = secList->neighbor->ID;
                                        tmpQ->next = NULL;
                                        endQ->next = tmpQ;
                                        endQ = tmpQ;
                                        tmpP->distN = cur_bfs;
                                        tmpP->pair->distance = curP->pair->distance;
                                        tmpP->pair->ID = curP->pair->ID;
                                        tmpP->nProp->prop[1] = curP->nProp->prop[1]+1;
                                        secList = secList->next;
                                    }
                                    curP->pair->ID = -1;
                                    curP->pair->distance = -1;
                                    tmpQ = curQ;
                                    curQ = curQ->next;
                                    free(tmpQ);
                                }
                                tmpNode = lookupNode(curQ->ID,g);
                                curP = tmpNode->obj;
                                edgeB += ((float)curP->pair->ID / curP->pair->distance); //current edgeBetweeness calculation
                                curP->pair->ID = -1;
                                while(curQ != NULL)
                                {
                                    tmpNode = lookupNode(curQ->ID,g);
                                    curP = tmpNode->obj;
                                    curP->pair->ID = -1;
                                    tmpQ = curQ;
                                    curQ = curQ->next;
                                    free(tmpQ);
                                }
                            }
                        }
                    }
                }
                if (edgeB > max)
                {
                    max = edgeB;
                    start = g->table[i][j]->ID;
                    end = mainList->neighbor->ID;
                }
                mainList = mainList->next;
            }
        }
    }
    for(i=0;i<g->size;i++) //distN and nProp->prop[1] (depth) initialization
    {
        for(j=0;j<g->position[i];j++)
        {
            tmpP = g->table[i][j]->obj;
            tmpP->distN = -1;
            tmpP->nProp->prop[1] = 0;
        }
    }
    *st = start;
    *e = end;
}

void removeEdge(int start,int end,int *sum_edges,Graph * g) //remove edge from the graph
{
    List * previous, *list;
    Node * tmpNode;
    Person * tmpP;
    tmpNode = lookupNode(start,g);
    tmpP = tmpNode->obj;
    list = tmpP->list;
    if (list->neighbor->ID == end) //one neighbor -> instant deletion
    {
        previous = tmpP->list;
        tmpP->list = tmpP->list->next;
        free(previous->neighbor);
        free(previous);
    }
    else
    {
        while(list != NULL)
        {
            previous = list;
            list = list->next;
            if (list->neighbor->ID == end)
            {
                previous->next = list->next;
                free(list->neighbor);
                free(list);
                break;
            }
        }
    }
    tmpP->edges--; //graph's number of edges --
    
    //same for the other node
    tmpNode = lookupNode(end,g);
    tmpP = tmpNode->obj;
    list = tmpP->list;
    if (list->neighbor->ID == start)
    {
        previous = tmpP->list;
        tmpP->list = tmpP->list->next;
        free(previous->neighbor);
        free(previous);
    }
    else
    {
        while(list != NULL)
        {
            previous = list;
            list = list->next;
            if (list->neighbor->ID == start)
            {
                previous->next = list->next;
                free(list->neighbor);
                free(list);
                break;
            }
        }
    }
    tmpP->edges--;
    *sum_edges -=2;
}

void markCommunities(Graph *g) //marking components of the graph grafo (for modularity calculation) using BFS
{
    int i,j;
    Queue * tmpQ, * endQ, * curQ;
    Node * tmpNode;
    Person * tmpP;
    List * mainList;
    int com = 1;
    for(i=0;i<g->size;i++)
    {
        for(j=0;j<g->position[i];j++)
        {
            tmpP = g->table[i][j]->obj;
            if (tmpP->nProp->prop[1] != 0)
                continue;
            *((int*)tmpP->nProp->prop[1]) = (uintptr_t)com;
            if (tmpP->list == NULL)
            {
                com++;
                continue;
            }
            tmpQ = malloc(sizeof(Queue));
            tmpQ->ID = tmpP->list->neighbor->ID;
            tmpQ->next = NULL;
            curQ = endQ = tmpQ;
            mainList = tmpP->list->next;
            while(mainList != NULL)
            {
                tmpQ = malloc(sizeof(Queue));
                tmpQ->ID = mainList->neighbor->ID;
                endQ->next = tmpQ;
                endQ = tmpQ;
                endQ->next = NULL;
                mainList = mainList->next;
            }
            while(curQ != NULL)
            {
                tmpNode = lookupNode(curQ->ID,g);
                tmpP = tmpNode->obj;
                *((int*)tmpP->nProp->prop[1]) = (uintptr_t)com;
                mainList = tmpP->list;
                while(mainList != NULL)
                {
                    tmpNode = lookupNode(mainList->neighbor->ID,g);
                    tmpP = tmpNode->obj;
                    if (tmpP->nProp->prop[1] != 0)
                    {
                        mainList = mainList->next;
                        continue;
                    }
                    tmpQ = malloc(sizeof(Queue));
                    tmpQ->ID = mainList->neighbor->ID;
                    endQ->next = tmpQ;
                    endQ = tmpQ;
                    endQ->next = NULL;
                    mainList = mainList->next;
                }
                tmpQ = curQ;
                curQ = curQ->next;
                free(tmpQ);
            }
            com++;
        }
    }
}

float computeModularity(Graph * g,int sum_edges) //modularity calculation
{
    float mod = 0.0;
    int x,y,z,w,tmp2;
    Node * tmpNode;
    Person * tmpP;
    List * list;
    int degree;
    uint64_t com;
    markCommunities(g);
    for(x = 0;x < g->size;x ++)
    {
        for (y = 0;y < g->position[x];y++)
        {
            tmpP = g->table[x][y]->obj;
            degree = tmpP->edges;
            com = (uint64_t)tmpP->nProp->prop[1];
            list = tmpP->list;
            //First, calculating modularity for immediate neighbors
            while(list != NULL)
            {
                tmpNode = lookupNode(list->neighbor->ID,g);
                tmpP = tmpNode->obj;
                if (!isPrevious(g->table[x][y]->ID,tmpNode->ID,g->hash,g->h_pointer,x))
                {
                    mod += (1-((float)tmpP->edges*degree/sum_edges));
                    tmpP->distN = 0;
                }
                list = list->next;
            }
            for (z = x;z < g->size;z++) //calculating modularity (rest of nodes)
            {
                if (x == z)
                    tmp2 = y + 1;
                else
                    tmp2 = 0;
                for (w = tmp2; w < g->position[z];w++)
                {
                    tmpP = g->table[z][w]->obj;
                    if (tmpP->distN != -1) //if he is alredy visited
                    {
                        tmpP->distN = -1;
                        continue;
                    }
                    if ((uint64_t)tmpP->nProp->prop[1] != com)
                        continue;
                    mod -= (float)tmpP->edges*degree/sum_edges; //neighbors -> remove from total sum
                }
            }
        }
    }
    if (sum_edges == 0)
        return -1;
    return mod/sum_edges;
}

Communities* findCom(Graph * g) //Final Communities
{
    int i,j,id = 1;
    Person * tmpP;
    Node * tmpNode;
    Communities * tmpC = NULL, * com = NULL;
    Queue * curQ, *startQ, *endQ;
    List * l;
    for(i = 0;i < g->size;i++)
    {
        for(j = 0;j < g->position[i];j++)
        {
            tmpP = g->table[i][j]->obj;
            if (tmpP->distN != -1)
            {
                tmpP->distN = -1;
                continue;
            }
            tmpP->distN = 0;
            tmpC = malloc(sizeof(Communities));
            tmpC->ID = id++;
            curQ = malloc(sizeof(Queue));
            curQ->ID = g->table[i][j]->ID;
            curQ->next = NULL;
            endQ = startQ = curQ;
            if (tmpP->list == NULL)
            {
                tmpP->distN = -1;
                tmpC->cliques = startQ;
                tmpC->next = com;
                com = tmpC;
                continue;
            }
            l = tmpP->list;
            while(l != NULL)
            {
                tmpNode = lookupNode(l->neighbor->ID,g);
                tmpP = tmpNode->obj;
                tmpP->distN = 0;
                endQ->next = malloc(sizeof(Queue));
                endQ->next->ID = l->neighbor->ID;
                endQ->next->next = NULL;
                endQ = endQ->next;
                l = l->next;
            }
            curQ = curQ->next;
            while(curQ != NULL)
            {
                tmpNode = lookupNode(curQ->ID,g);
                tmpP = tmpNode->obj;
                l = tmpP->list;
                while(l != NULL)
                {
                    tmpNode = lookupNode(l->neighbor->ID,g);
                    tmpP = tmpNode->obj;
                    if (tmpP->distN != -1)
                    {
                        l = l->next;
                        continue;
                    }
                    tmpP->distN = 0;
                    endQ->next = malloc(sizeof(Queue));
                    endQ->next->ID = l->neighbor->ID;
                    endQ->next->next = NULL;
                    endQ = endQ->next;
                    l = l->next;
                }
                curQ = curQ->next;
            }
            tmpP = g->table[i][j]->obj;
            tmpP->distN = -1;
            tmpC->cliques = startQ;
            tmpC->next = com;
            com = tmpC;
        }
    }
    return com;
}

Communities* GirvanNewman(double threshold, Graph* g)
{
    printf("\nGirvanNewman starts!\n");
    int i,j,start = 0,end = 0,sum_edges = 0;
    float mod = 0.0,pmod = -1;
    Person * tmpP;
    for(i = 0; i < g->size;i++) //calculating number of edges
    {
        for(j = 0;j < g->position[i];j++)
        {
            tmpP = g->table[i][j]->obj;
            sum_edges += tmpP->edges;
        }
    }
    edgeBetweeness(g,&start,&end);
    removeEdge(start,end,&sum_edges,g);
    while(mod < threshold && pmod < mod)
    {
        pmod = mod;
        edgeBetweeness(g,&start,&end);
        mod = computeModularity(g,sum_edges);
        if (pmod < mod)
            removeEdge(start,end,&sum_edges,g);
    }
    return findCom(g);
}