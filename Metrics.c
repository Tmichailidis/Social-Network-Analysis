//
//  Metrics.c
//  Software
//
//  Created by Theodore Michailidis on 12/14/14.
//  Copyright (c) 2014 Theodore Michailidis. All rights reserved.
//

#include "Metrics.h"

void degreeDistribution(Graph * g)
{
    DegreeList * list = NULL, * tmp, * start = NULL;
    Person * p;
    int i,j;
    for (i = 0;i < g->size;i++)
    {
        for (j = 0; j < g->position[i];j++)
        {
            p = g->table[i][j]->obj;
            if(p->edges == 0)
                continue;
            tmp = NULL;
            if (start == NULL)
            {
                tmp = malloc(sizeof(DegreeList));
                tmp->counter = 1; //first encounter of this size of edges
                tmp->degree = p->edges;
                tmp->next = NULL;
                start = tmp;
                continue;
            }
            list = start;
            while(list != NULL)
            {
                if(list->degree == p->edges)
                {
                    list->counter++;
                    break;
                }
                if(list->degree < p->edges)
                {
                    if (list->next != NULL)
                    {
                        if (list->next->degree > p->edges)
                        {
                            tmp = malloc(sizeof(DegreeList));
                            tmp->counter = 1;
                            tmp->degree = p->edges;
                            tmp->next = list->next;
                            list->next = tmp;
                            break;
                        }
                        else
                            list = list->next;
                    }
                    else
                    {
                        tmp = malloc(sizeof(DegreeList));
                        tmp->counter = 1;
                        tmp->degree = p->edges;
                        tmp->next = NULL;
                        list->next = tmp;
                        break;
                    }
                }
                else
                {
                    tmp = malloc(sizeof(DegreeList));
                    tmp->counter = 1;
                    tmp->degree = p->edges;
                    tmp->next = start;
                    start = tmp;
                    break;
                }
            }
        }
    }
    
    //FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
    //fprintf(gnuplotPipe, "set xrange [0:%d]\n",10); 
    //fprintf(gnuplotPipe, "set yrange [0.01:1.00]\n"); 
    //fprintf(gnuplotPipe, "plot '-' \n");
    
    while(start != NULL){
        //fprintf(gnuplotPipe, "%d %lf\n",start->degree,(double)start->counter/g->nodes_sum);
        tmp = start;
        start = start->next;
        free(tmp);
    }
    //fprintf(gnuplotPipe, "e");
}

int diameter(Graph * g)
{
    int k,l,m,n,max = 0,tmp,tmp2;
    Person * p;
    for (k = 0;k < g->size;k++)
        for (l = 0; l < g->position[k];l++)
        {
            p = g->table[k][l]->obj;
            if(p->list == NULL)
                break;
            for (m = k;m < g->size;m++)
            {
                if (k == m)
                    tmp2 = l+1;
                else
                    tmp2 = 0;
                for (n = tmp2; n < g->position[m];n++)
                {
                    if ((tmp = reachNode1(g->table[k][l]->ID,g->table[m][n]->ID,g)) > max && tmp != INFINITY_REACH_NODE)
                        max = tmp;
                }
            }
        }
    return max;
}

double averagePathLength(Graph * g)
{
    int k,l,m,n,counter = 0,tmp,tmp2;
    double sum = 0.0;
    Person * p;
    for (k = 0;k < g->size;k++)
    {
        for (l = 0; l < g->position[k];l++)
        {
            p = g->table[k][l]->obj;
            if(p->list == NULL)
                break;
            for (m = k;m < g->size;m++)
            {
                if (k == m)
                    tmp2 = l+1;
                else
                    tmp2 = 0;
                for (n = tmp2; n < g->position[m];n++)
                {
                    if((tmp = reachNode1(g->table[k][l]->ID,g->table[m][n]->ID,g)) != INFINITY_REACH_NODE){
                        sum += tmp;
                        counter++;
                    }
                }
            }
        }
    }
    if (counter == 0)
        return 0;
    return sum/counter;
}

double density(Graph *g)
{
    int i,j,k,index;
    List * tmp;
    double sum = 0.0;
    Person * p;
    for(i = 0;i < g->size;i ++)
    {
        for (j = 0;j < g->position[i];j++)
        {
            p = g->table[i][j]->obj;
            tmp = p->list;
            for (k = 0; k < p->edges; k++)
            {
                index = tmp->neighbor->ID %g->hash;
                if (g->h_pointer > index) //Before hash_pointer
                    index = tmp->neighbor->ID%(2*g->hash);
                if ((index == i && tmp->neighbor->ID > g->table[i][j]->ID) || index > i)
                    sum += 2;
                tmp = tmp->next;
            }
        }
    }
    return (2*sum)/(g->nodes_sum*(g->nodes_sum-1));
}

double closenessCentrality(Node * n, Graph * g)
{
    double sum = 0.0;
    ResultSet* res = reachNodeN(n->ID,g);
    Pair pair;
    while (next(res, &pair))
        sum += (1.0/pair.distance);
    if (g->nodes_sum == 1)
        return 0;
    return sum/(g->nodes_sum-1);
}

Queue * addBeQ(Queue * q,Node * n,int d,int p,int f) //f:number of paths from centrality node,p:number of the rest paths
{
    Queue * tmp;
    tmp = malloc(sizeof(Queue));
    Person * person = n->obj;
    tmp->ID = n->ID;
    person->pair->distance = d;
    person->pair->ID = f;
    person->distN = p;
    tmp->next = NULL;
    if (q != NULL)
        q->next = tmp;
    return tmp;
}

void freeCentralQueue(Queue * q,Graph * g)
{
    Queue * tmp = NULL;
    Node * n = NULL;
    Person * p;
    while(q != NULL)
    {
        n = lookupNode(q->ID,g);
        p = n->obj;
        p->distN = -1;
        p->pair->ID = 0;
        tmp = q;
        q = q->next;
        free(tmp);
    }
}

double betweennessCentrality(Node* n, Graph* g)
{
    int i,j,k=0,x,y,tmp2,std,sid,itd; //sid: source-i distance,std: source-target distance,i-target distance
    Queue * q = NULL, * start, * end;
    List * list;
    Node * n1, * n2;
    Person * p1, *p2;
    double sum = 0.0;
    for(i = 0;i < g->size;i ++)
    {
        for (j = 0;j < g->position[i];j++)
        {
            for (x = i;x < g->size;x++)
            {
                if (n->ID == g->table[i][j]->ID)
                    continue;
                if (i == x)
                    tmp2 = j + 1;
                else
                    tmp2 = 0;
                for (y = tmp2; y < g->position[x];y++)
                {
                    if (n->ID == g->table[x][y]->ID)
                        continue;
                    std = reachNode1(g->table[i][j]->ID,g->table[x][y]->ID,g);
                    if (((sid = reachNode1(g->table[i][j]->ID,n->ID,g)) > std) || ((itd = reachNode1(g->table[x][y]->ID,n->ID,g)) != std-sid))
                        continue;
                    q = addBeQ(q,g->table[i][j],std,1,0);
                    end = start = q;
                    while(q != NULL)
                    {
                        n1 = lookupNode(q->ID,g);
                        p1 = n1->obj;
                        if (p1->pair->distance == 0 && n1 == g->table[x][y])
                        {
                            k++;
                            sum += ((double)p1->pair->ID)/(p1->distN + p1->pair->ID);
                        }
                        list = p1->list;
                        while(list != NULL)
                        {
                            n2 = lookupNode(list->neighbor->ID, g);
                            p2 = n2->obj;
                            if(p2->distN == -1) //if node is not in the list
                            {
                                if (n1 == n) //if parent node is centrality node
                                    end = addBeQ(end,n2,p1->pair->distance-1,0,p1->distN);
                                else
                                    end = addBeQ(end,n2,p1->pair->distance-1,p1->distN,p1->pair->ID);
                            }
                            else if (p2->pair->distance == p1->pair->distance-1) //if node is in the list
                            {
                                if (n1 == n) //if parent node is centrality node
                                    p2->pair->ID = p1->distN;
                                else //update paths
                                {
                                    p2->pair->ID += p1->pair->ID;
                                    p2->distN += p1->distN;
                                }
                            }
                            list = list->next;
                        }
                        q = q->next;
                    }
                    freeCentralQueue(start,g);
                }
            }
        }
    }
    if (g->nodes_sum == 2 || g->nodes_sum == 1)
        return 0;
    return sum/((g->nodes_sum-2) * (g->nodes_sum-1) / 2);
}

int generalCC(char mode,Graph* g)
{
    int i,j,counter = 0,length,max = 0;
    ResultSet * res;
    Pair pair;
    Person * p;
    Node * n;
    for(i = 0;i < g->size;i ++)
    {
        for (j = 0;j < g->position[i];j++)
        {
            p = g->table[i][j]->obj;
            if (p->pair->ID !=  -1)
            {
                p->pair->ID = -1;
                continue;
            }
            counter++;
            res = reachNodeN(g->table[i][j]->ID,g);
            length = 1;
            while (next(res, &pair))
            {
                n = lookupNode(pair.ID,g);
                p = n->obj;
                p->pair->ID = 1;
                length++;
            }
            if (max < length)
                max = length;
        }
    }
    if (mode == 0)
        return counter;
    else
        return max;
}

int numberOfCCs(Graph * g)
{
    return generalCC(0,g);
}

int maxCC(Graph* g)
{
    return generalCC(1,g);
}
