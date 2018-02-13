#include "stalking.h"

Graph * createPosts(char * file)    //Posts graph creation
{
    FILE * fp = fopen(file,"r");
    if (fp == NULL) perror ("Error opening file");
    char buffer[200],* s;
    int i = 0, *creator;
    Node * postNode;
    fgets(buffer,200,fp);   // first line has useless information
    Graph * postGraph = createGraph(M,C);
    while(fgets(buffer,200,fp)!= NULL)
    {
        s = strtok (buffer,"|");
        postNode = malloc(sizeof(Node));
        postNode ->ID = atoi(s);
        creator = malloc(4);
        *creator = atoi(strtok (NULL, "\n"));   //post creator
        postNode->obj = creator;
        insertNode(postNode,postGraph);
        i++;
    }
    fclose(fp);
    return postGraph;
}

int freePP(Graph *postG,Graph *personG) //freeing posts & people graphs
{
    int i,j;
    for (i=0;i<postG->size;i++)
    {
        for(j=0;j<postG->position[i];j++)
        {
            free(postG->table[i][j]->obj);
            free(postG->table[i][j]);
        }
        free(postG->table[i]);
    }
    free(postG->table); 
    free(postG->cell_size);
    free(postG->position);
    free(postG); 
    Likes * tmp, *likes;
    for (i=0;i<personG->size;i++)
    {
        for(j=0;j<personG->position[i];j++)
        {
            likes = personG->table[i][j]->obj;
            while(likes != NULL)
            {
                tmp = likes;
                likes = likes->next;
                free(tmp);
            }
            free(personG->table[i][j]);
        }
        free(personG->table[i]);
    }
    free(personG->table); 
    free(personG->cell_size);
    free(personG->position);
    free(personG); 
    return 1;
}

void findStalkers(Graph * g,Graph * PostGraph,Graph * stalkerGraph,char *file,int x)
{
    Likes * likes, * ltmp = NULL,*previous;
    FILE * fp = fopen(file,"r");
    if (fp == NULL) perror ("Error opening file");
    char buffer[200];
    int i = 0,ID,postID;
    Node * likeNode = NULL, *post,*tmp, * new_stNode;
    Person * p,*ptmp;
    List * l;
    Properties * prop;
    Graph * PersonGraph = createGraph(M,C);
    fgets(buffer,200,fp);
    while(fgets(buffer,200,fp)!= NULL)
    {
        i++;
        ID = atoi(strtok (buffer,"|"));
        postID = atoi(strtok (NULL, "|"));
        if((post = lookupNode(postID,PostGraph))==NULL)
        	continue;
        if (*(int *)(post->obj) == ID) //self-like
            continue;
        tmp = lookupNode(ID,g);
        p = tmp->obj;
        if (p->distN != -1) //if p is stalker
            continue;
        l = p->list;
        while(l != NULL) //direct neighbors
        {
            if (l->neighbor->ID == *(int *)(post->obj)) 
                break;
            l = l->next;
        }
        if (l != NULL) 
            continue;
        tmp = lookupNode(ID,PersonGraph);
        previous = NULL;
        ltmp = NULL;
        if (tmp != NULL) //checking if tmp is a node on "people graph"
            ltmp = tmp->obj;
        if (ltmp != NULL) //checking his/her likes
        {
            while(ltmp->next != NULL)
            {
                if (ltmp->creatorID == *(int *)(post->obj)) //checking if there exists a previous like in the same person
                {
                    ltmp->counter++;
                    if (x < ltmp->counter) //checking number of likes in a person she/he doesn't personally know
                    {
                        p->distN = 1; //stalker
                        prop = createProperties(1);
                        new_stNode = createPerson(ID,prop);
                        insertNode(new_stNode, stalkerGraph);
                        ptmp = new_stNode->obj;
                        ptmp->nProp->prop[0] = NULL;
                        ptmp->prop_num = 0;
                    }
                    break;
                }
                ltmp = ltmp->next;
            }
            previous = ltmp;
            if ((ltmp->creatorID == *(int *)(post->obj)) && p->distN != 1) //checking last node of the list
            {
                ltmp->counter++;
                if (x < ltmp->counter)
                {
                    p->distN = 1; // stalker
                    prop = createProperties(1);
                    new_stNode = createPerson(ID,prop);
                    insertNode(new_stNode, stalkerGraph);
                    p = new_stNode->obj;
                    p->nProp->prop[0] = NULL;
                    p->prop_num = 0;
                }
            }
            else
                ltmp = NULL;
        }
        if (ltmp != NULL) 
            continue;
        if (p->distN == 1)
            continue;
        if (x == 0) 
        {
            p->distN = 1; // stalker
            prop = createProperties(1);
            new_stNode = createPerson(ID,prop);
            insertNode(new_stNode, stalkerGraph);
            p = new_stNode->obj;
            p->nProp->prop[0] = NULL;
            p->prop_num = 0;
            continue;
        }
        likes = malloc(sizeof(Likes));
        likes->counter = 1;
        likes->creatorID = *(int *)(post->obj); //post creator (from postGraph)
        likes->next = NULL;
        if (previous != NULL) 
            previous->next = likes;
        if(tmp == NULL)
        {
            likeNode = malloc(sizeof(Node));
            likeNode->ID = ID;
            likeNode->obj = likes;
            insertNode(likeNode,PersonGraph); //insertNode
        }
    }
    fclose(fp);
    freePP(PostGraph,PersonGraph); 
}

void addForums(char * forumF, Graph * stalkersGraph)    //forums of every stalker
{
    FILE * fp = fopen(forumF,"r");
    Node * stalker;
    Queue * tmp;
    Person * obj;
    char buffer[200];
    int forum;
    fgets(buffer,200,fp); //useless line
    while(fgets(buffer,200,fp)!= NULL)
    {
        forum = atoi(strtok (buffer,"|"));
        if ((stalker = lookupNode(atoi(strtok (NULL,"|")), stalkersGraph))== NULL) //not a stalker
            continue;
        obj = stalker->obj;
        tmp = malloc(sizeof(Queue));
        tmp->ID = forum;    //forum ID
        if (obj->nProp->prop[0] != NULL)
            tmp->next = (Queue *)obj->nProp->prop[0];
        else
            tmp->next = NULL;
        obj->nProp->prop[0] = (void *) tmp; //stalker forum list
    }
    fclose(fp);
}

void addFellowStalkers(Graph * g,Graph * stalkersGraph) //stalker edges
{
    int i,j,index;
    Node * n1,*n2;
    Person *p1;
    List * l;
    Edge * e;
    for (i = 0;i< stalkersGraph->size;i++)
    {
        for(j = 0;j<stalkersGraph->position[i];j++)
        {
            n1 = lookupNode(stalkersGraph->table[i][j]->ID,g); //searching node on graph
            p1 = n1->obj;
            p1->distN = -1;
            if ((l = p1->list)==NULL) //checking for neighbors
                continue;
            while(l != NULL)
            {
                if((n2 = lookupNode(l->neighbor->ID,stalkersGraph))==NULL) //checking if the neighbor is an identified stalker
                {
                    l = l->next;
                    continue;
                }
                index = l->neighbor->ID %stalkersGraph->hash;
                if (stalkersGraph->h_pointer > index) 
                    index = l->neighbor->ID%(2*stalkersGraph->hash);
                if (index < i || (index == i && l->neighbor->ID < stalkersGraph->table[i][j]->ID))
                {
                    l = l->next;
                    continue;
                }
                e = createEdge(0,l->neighbor->ID,NULL);
                insertEdge(stalkersGraph->table[i][j]->ID,e,stalkersGraph);
                e = createEdge(0,stalkersGraph->table[i][j]->ID,NULL);
                insertEdge(l->neighbor->ID,e,stalkersGraph);
                l = l->next;
            }
        }
    }
}

void destroyStalkers(Graph *st) //stalkers graph deletion
{
    int i,j;
    Queue * q,*tmp;
    Person * p;
    for(i=0;i<st->size;i++)
    {
        for(j=0;j<st->position[i];j++)
        {
            p = st->table[i][j]->obj;
            q = p->nProp->prop[0];
            while(q != NULL)   
            {
                tmp = q;
                q = q->next;
                free(tmp);
            }
        }
    }
    destroyGraph(st);  
}

Graph* getTopStalkers(int k, int x, int centralityMode, Graph* g, Stalkers* st)
{
    Graph * PostGraph = createPosts("post_hasCreator_person.csv");
    Graph * stalkersGraph = createGraph(M,C);
    findStalkers(g,PostGraph,stalkersGraph,"person_likes_post.csv",x);
    addForums("forum_hasMember_person.csv",stalkersGraph);
    int i,j,entry=0;
    double temp;
    for(i = 0;i < k;i++)    //initialization
    {
        st[i].ID = 0;
        st[i].score = 0.0;
    }
    addFellowStalkers(g,stalkersGraph);
    if(centralityMode == 1) //calculating stalking score with closenessCentrality 
    {
        for(i=0;i<stalkersGraph->size; i++)
        {
            for(j=0;j<stalkersGraph->position[i];j++)
            {
                if (entry < k) //top k stalkers not available yet
                {
                    st[entry].ID = stalkersGraph->table[i][j]->ID;
                    st[entry].score = closenessCentrality(stalkersGraph->table[i][j], g);
                    entry++;
                }
                else
                {
                    st = insertionS(st,entry);  //sorting
                    temp = closenessCentrality(stalkersGraph->table[i][j], g);
                    if (st[0].score >= temp)    // low score -> ignore
                        continue;
                    st[0].ID = stalkersGraph->table[i][j]->ID;  //replace stalker with lowest score
                    st[0].score = temp;
                }
            }
        }
    }
    else    //calculating stalking score with betweennessCentrality 
    {
        for(i=0;i<stalkersGraph->size; i++)
        {
            for(j=0;j<stalkersGraph->position[i];j++)
            {
                if (entry < k)  //top k stalkers not available yet
                {
                    st[entry].ID = stalkersGraph->table[i][j]->ID;
                    st[entry].score = betweennessCentrality(stalkersGraph->table[i][j], stalkersGraph);
                    entry++;
                }
                else
                {
                    st = insertionS(st,entry);  //sorting
                    temp = betweennessCentrality(stalkersGraph->table[i][j], stalkersGraph);
                    if (st[0].score >= temp)    // low score -> ignore
                        continue;
                    st[0].ID = stalkersGraph->table[i][j]->ID;  //replace stalker with lowest score
                    st[0].score = temp;
                }
            }
        }
    }
    st = insertionS(st,entry);
    return stalkersGraph;
}

void Metrics(Graph * g) //metrics 
{
    //degreeDistribution(g);
    printf("Diameter is %d\n",diameter(g));
    printf("AveragePathLength is %f\n",averagePathLength(g));
    printf("numberOfCCs is %d\n",numberOfCCs(g));
    printf("Max of CCs is %d\n",maxCC(g));
    printf("Density is %f\n",density(g));
    putchar('\n');
}
