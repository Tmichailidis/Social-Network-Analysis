#include "trust.h"

Graph * createPostForum(char * file,char * file2, int forumID)
{
    FILE * fp = fopen(file,"r");
    if (fp == NULL) perror ("Error opening file");
    char buffer[200];
    int i = 0, *creator,ID;
    Node * postNode;
    fgets(buffer,200,fp);
    Graph * postGraph = createGraph(M,C);
    while(fgets(buffer,200,fp)!= NULL)
    {
    	ID = atoi(strtok (buffer,"|"));
        if(ID != forumID)
        	continue;
        // add to post forum
        postNode = malloc(sizeof(Node));
        postNode ->ID = atoi(strtok (NULL,"\n"));
        postNode->obj = NULL;
        insertNode(postNode,postGraph);
        i++;
    }
    fclose(fp);
    //finding posts creators
	fp = fopen(file2, "r"); //file2 = post_hasCreator_person
    if (fp == NULL) perror ("Error opening file");
	fgets(buffer,200,fp);
	while(fgets(buffer,200,fp)!= NULL)
	{
		if((postNode = lookupNode(atoi(strtok (buffer,"|")), postGraph)) != NULL)
		{
			creator = malloc(sizeof(int));
			*creator = atoi(strtok (NULL, "\n"));
			postNode->obj = creator;
		}
	}
	fclose(fp);
    return postGraph;
}

Graph * CreateComments(char * file, char * file2, Graph * PostGraph)    //finding comments and creators of comments
{
	FILE* fp = fopen(file,"r"); //file = comment_replyOf_post
	Node * n;
	int *creator;
	Graph * CommentsGraph = createGraph(M,C);
	if (fp == NULL)
	{
		perror ("Error opening file");
		exit(-1);
	}
	char buffer[200],*s;
	fgets(buffer,200,fp);
	while(fgets(buffer,200,fp)!= NULL)
	{
		s = strtok (buffer,"|");
		if((n = lookupNode(atoi(strtok(NULL,"\n")), PostGraph)) != NULL)  
		{
			n = malloc(sizeof(Node));
			n->ID = atoi(s);
			n->obj = NULL;
			insertNode(n,CommentsGraph);
		}
	}
	fclose(fp);
	fp = fopen(file2, "r"); //file2 = comment_hasCreator_person
	fgets(buffer,200,fp);
	while(fgets(buffer,200,fp)!= NULL)
	{
		if((n = lookupNode(atoi(strtok (buffer,"|")), CommentsGraph)) != NULL) 
		{
			creator = malloc(4);
			*creator = atoi(strtok (NULL, "\n"));
			n->obj = creator;
		}
	}
	fclose(fp);
	return CommentsGraph;
}

void insertLikes(char * file, Graph * postGraph, Graph * trustGraph, Graph * g)
{
	FILE * fp = fopen(file,"r"); //person_likes_post
	if (fp == NULL)
	{
		perror ("Error opening file");
		exit(-1);
	}
	int ID;
	char buffer[200];
	Node * n, * n1;
	Person * p, * p1;
	List * l;
	LikeCom * likes, * tmp, * previous;
	int creatorID;
	fgets(buffer,200,fp);
	while(fgets(buffer,200,fp)!= NULL)
	{
		ID = atoi(strtok (buffer,"|"));
		n = lookupNode(atoi(strtok(NULL,"|")),postGraph);
		if (n == NULL)
			continue;
		creatorID = *(int *)n->obj;
		n = lookupNode(ID,g);
		p = n->obj;
		l = p->list;
		while(l != NULL)     //checking neighbors
		{
			if(l->neighbor->ID != creatorID || lookupNode(l->neighbor->ID,trustGraph) == NULL) //if the post creator is not a neighbor of the "liker", continue
			{
				l = l->next;
				continue;
			}
			n1 = lookupNode(ID,trustGraph);
			p1 = n1->obj;
			if(p1->nProp == NULL)
			{
				p1->nProp = (void*)createProperties(3);
				p1->nProp->prop[0] = (void *)1; //sum of likes
				p1->nProp->prop[1] = 0; //sum of replies
				p1->nProp->prop[2] = NULL; // list
				likes = malloc(sizeof(LikeCom));
				likes->creatorID = creatorID;
				likes->likes_count = 1;
				likes->replies_count = 0;
				likes->next = NULL;
				p1->nProp->prop[2] = likes;
				l = l->next;
				continue;
			}
			p1->nProp->prop[0]++;
			likes = p1->nProp->prop[2];
			previous = NULL;
			while(likes != NULL) //while the likes list is not empty, search for a previous like in the "creator"
			{
				if(likes->creatorID == creatorID)
				{
					likes->likes_count++;
					break;
				}
				if(likes->creatorID > creatorID)
				{
					tmp = malloc(sizeof(LikeCom));
					tmp->creatorID = creatorID;
					tmp->likes_count = 1;
					tmp->replies_count = 0;
					tmp->next = likes;
					if (previous != NULL)
						previous->next = tmp;
					else
						p1->nProp->prop[2] = tmp;
					break;
				}
				else if (likes->next == NULL)
				{
					likes->next = malloc(sizeof(LikeCom));
					likes->next->creatorID = creatorID;
					likes->next->likes_count = 1;
					likes->next->replies_count = 0;
					likes->next->next = NULL;
					break;
				}
				else
				{
					previous = likes;
					likes = likes->next;
				}
			}
			l = l->next;
		}
	}
	fclose(fp);
}

void insertComments(char * file, Graph * CommentsGraph, Graph * postGraph,  Graph * trustGraph, Graph * g)
{
	FILE * fp = fopen(file,"r"); //person_likes_post
	if (fp == NULL)
	{
		perror ("Error opening file");
		exit(-1);
	}
	int commentID, com_creatorID, post_creatorID;
	char buffer[200];
	Node * n, * n1;
	Person * p, * p1;
	List * l;
	LikeCom * replies, * tmp, * previous;
	fgets(buffer,200,fp);
	while(fgets(buffer,200,fp)!= NULL)
	{
		commentID = atoi(strtok (buffer,"|"));
		n1 = lookupNode(commentID,CommentsGraph);
		if (n1 == NULL) //comment does not exist in CommentsGraph
			continue;
		n = lookupNode(atoi(strtok(NULL,"|")),postGraph);
		if (n == NULL) //post does not exist in postGraph
			continue;
		if(n->obj == NULL || n1->obj == NULL)
			continue;
		post_creatorID = *(int *)n->obj;
		com_creatorID = *(int *)n1->obj;
		if((n = lookupNode(com_creatorID,g))==NULL)  // comment creator in graph g
			continue;
		p = n->obj;
		l = p->list;
		while(l != NULL)
		{
			if(l->neighbor->ID != post_creatorID || lookupNode(l->neighbor->ID,trustGraph) == NULL) 
			{
				l = l->next;
				continue;
			}
			n = lookupNode(com_creatorID,trustGraph); 
			p1 = n->obj;
			if(p1->nProp == NULL)
			{
				p1->nProp = (void*)createProperties(3);
				p1->nProp->prop[0] = 0; 
				p1->nProp->prop[1] = (void *) 1; 
				p1->nProp->prop[2] = NULL; 
				replies = malloc(sizeof(LikeCom));
				replies->creatorID = post_creatorID;
				replies->likes_count = 0;
				replies->replies_count = 1;
				replies->next = NULL;
				p1->nProp->prop[2] = replies;
				l = l->next;
				continue;
			}
			p1->nProp->prop[1]++;
			replies = p1->nProp->prop[2];
			previous = NULL;
			while(replies != NULL) //while the replies list is not empty, search for a previous like in the "creator"
			{
				if(replies->creatorID == post_creatorID)
				{
					replies->replies_count++;
					break;
				}
				if(replies->creatorID > post_creatorID)
				{
					tmp = malloc(sizeof(LikeCom));
					tmp->creatorID = post_creatorID;
					tmp->likes_count = 0;
					tmp->replies_count = 1;
					tmp->next = replies;
					if (previous != NULL)
						previous->next = tmp;
					else
						p1->nProp->prop[2] = tmp;
					break;
				}
				else if (replies->next == NULL)
				{
					replies->next = malloc(sizeof(LikeCom));
					replies->next->creatorID = post_creatorID;
					replies->next->likes_count = 0;
					replies->next->replies_count = 1;
					replies->next->next = NULL;
					break;
				}
				else
				{
					previous = replies;
					replies = replies->next;
				}
			}
			l = l->next;
		}
	}
	int i,j;
	for(i=0;i<trustGraph->size;i++){
		for(j=0;j<trustGraph->position[i];j++)
		{
			p1 = trustGraph->table[i][j]->obj;
			if (p1->nProp == NULL)
				continue;
			replies = p1->nProp->prop[2];
			while(replies != NULL)
				replies = replies->next;
		}
	}
	fclose(fp);
}

void addTrust(Graph * trustGraph, Graph * g)	//insert trust edges in  trustGraph
{
	int i,j;
	Node * n;
	Person * p;
	List * l;
	Properties * props;
	Edge * e;
    LikeCom * tmp,*lc;
	for(i=0; i < trustGraph->size; i++)	//for every node in trustGraph, adding edges
	{
		for(j=0; j < trustGraph->position[i]; j++)
		{
			n = lookupNode(trustGraph->table[i][j]->ID,g);
			p = n->obj;
			l = p->list;
			while(l != NULL)
			{
				if((n = lookupNode(l->neighbor->ID,trustGraph)) == NULL)
				{
					l = l->next;
					continue;
				}
				props = createProperties(2);
				setStringProperty("trust",0,props);
                props->prop[1] = malloc(sizeof(double));
				e = createEdge(0,l->neighbor->ID,props);
				insertEdge(trustGraph->table[i][j]->ID,e,trustGraph);
				l = l->next;
			}
		}
	}
    LikeCom * pivot;
    for(i=0; i < trustGraph->size; i++) //calculating weight
    {
        for(j=0; j < trustGraph->position[i]; j++)
        {
            p = trustGraph->table[i][j]->obj;
            if (p->nProp == NULL)
            {
                p->prop_num = 2;
                p->nProp = (void*)createProperties(1);
                p->nProp->prop[0] = malloc(sizeof(double));
                continue;
            }
            pivot = p->nProp->prop[2];
            l = p->list;
            while(l != NULL)
            {
                while(pivot != NULL)
                {
                    if (l->neighbor->ID == pivot->creatorID)
                    {
                        *(double*)l->neighbor->eProp->prop[1] = 0;
                        if((uintptr_t)p->nProp->prop[0] != 0)
                            *(double *) l->neighbor->eProp->prop[1] = 0.3*pivot->likes_count/(uintptr_t)p->nProp->prop[0]; //calculating likes weight
                        if((uintptr_t)p->nProp->prop[1] != 0)
                            *(double *) l->neighbor->eProp->prop[1] += 0.7*pivot->replies_count/(uintptr_t)p->nProp->prop[1]; //calculating replies weight
                        pivot = pivot->next;
                    }
                    l = l->next;
                }
                if (l != NULL)
                    l = l->next;
            }
            lc = p->nProp->prop[2];
            while(lc!=NULL) //free
            {
                tmp = lc;
                lc = lc->next;
                free(tmp);
            }
            p->prop_num = 2;
            p->nProp->prop[0] = malloc(sizeof(double)); //allocating trust
        }
    }
}

void freePCGraph(Graph * g)
{
	int i,j;
    for(i=0;i<g->size;i++)
    {
        for(j=0;j<g->position[i];j++)
        {
            free(g->table[i][j]->obj);
            free(g->table[i][j]);
        }
        free(g->table[i]);
    }
    free(g->table);
    free(g->cell_size);
    free(g->position);
    free(g);
}

Graph* buildTrustGraph(int forum, Graph* g)
{
	Graph * trustGraph = createGraph(M,C);
    char buffer[200];
    Node * n;
    FILE * fp = fopen("forum_hasMember_person.csv","r");
	if (fp == NULL)
	{
		perror ("Error opening file");
		exit(-1);
	}
	fgets(buffer,200,fp);
	while(fgets(buffer,200,fp)!= NULL) //insert users of the forum
	{
		if (forum == atoi(strtok (buffer,"|")))
		{
			n = createPerson(atoi(strtok (NULL,"|")),NULL);
			insertNode(n,trustGraph);
		}
	}
	fclose(fp);
    Graph * PostGraph = createPostForum("forum_containerOf_post.csv","post_hasCreator_person.csv",forum); //creating posts hash_map
	Graph * CommentGraph = CreateComments("comment_replyOf_post.csv","comment_hasCreator_person.csv",PostGraph);
	insertLikes("person_likes_post.csv", PostGraph, trustGraph,g);
	insertComments("comment_replyOf_post.csv", CommentGraph, PostGraph, trustGraph, g);
    addTrust(trustGraph,g);
    freePCGraph(PostGraph);
    freePCGraph(CommentGraph);
	return trustGraph;
}

double estimateTrust(Node * a, Node * b, Graph * trustGraph) //calculating trust
{
    Person * p = a->obj,*p1;
    List * l = p->list;
    Queue * q = NULL, * tmp = NULL,*end = NULL;
    Node * n = NULL,*n1 = NULL;
    if(a == b)
        return 0.0;
    if(l == NULL)
        return 0.0;
    p->distN = 0;
    q = malloc(sizeof(Queue));
    q->ID = l->neighbor->ID;
    q->next = NULL;
    n = lookupNode(q->ID,trustGraph);
    p = n->obj;
    p->distN = 1;
    *(double*)p->nProp->prop[0] = *(double *)l->neighbor->eProp->prop[1];
    l = l->next;
    end = q;
    while(l != NULL)
    {
        end->next = malloc(sizeof(Queue));
        end->next->ID = l->neighbor->ID;
        end->next->next = NULL;
        end = end->next;
        n = lookupNode(l->neighbor->ID,trustGraph);
        p = n->obj;
        p->distN = 1;   //init trust
        *(double*)p->nProp->prop[0] = *(double *)l->neighbor->eProp->prop[1]; //adding trust
        l = l->next;
    }
    while(q->ID != b->ID)
    {
        n = lookupNode(q->ID,trustGraph);
        p = n->obj;
        l = p->list;
        while(l != NULL)
        {
            n1 = lookupNode(l->neighbor->ID,trustGraph);
            p1 = n1->obj;
            if(p1->distN == -1) 
            {
                end->next = malloc(sizeof(Queue));
                end->next->ID = l->neighbor->ID;
                end->next->next = NULL;
                end = end->next;
                p1->distN = p->distN+1;  //adding trust
                *(double*)p1->nProp->prop[0] = (*(double *)l->neighbor->eProp->prop[1])*(*(double *)p->nProp->prop[0]); //edge trust * trust till now
            }
            else if(p1->distN == p->distN+1 && (*(double *)l->neighbor->eProp->prop[1])*(*(double *)p->nProp->prop[0] > *(double *)p1->nProp->prop[0])) //if current trust and the product > trust in p1
            {
                *(double *)p1->nProp->prop[0] = (*(double *)l->neighbor->eProp->prop[1])*(*(double *)p->nProp->prop[0]);
            }
            else if(p1->distN > p->distN+1)
            {
                *(double *)p1->nProp->prop[0] = (*(double *)l->neighbor->eProp->prop[1])*(*(double *)p->nProp->prop[0]);
                p1->distN = p->distN+1;
            }
            l = l->next;
        }
        tmp = q;
        q = q->next;
        free(tmp);
    }
    n = lookupNode(q->ID,trustGraph);
    p = n->obj;
    double res = *(double *)p->nProp->prop[0];
    while(q != NULL)
    {
        tmp = q;
        q = q->next;
        free(tmp);
    }
    int i,j;
    for(i=0;i<trustGraph->size;i++)
    {
        for(j=0;j<trustGraph->position[i];j++)
        {
            p = trustGraph->table[i][j]->obj;
            p->distN = -1;
        }
    }
    return res;
}

void destroyTrustGraph(Graph * trustGraph)
{
    List *l;
    int i,j;
    Person *p;
    for(i=0; i<trustGraph->size;i++)
    {
    	for(j=0; j<trustGraph->position[i]; j++)
    	{
    		p = trustGraph->table[i][j]->obj;
    		l = p->list;
    		while(l != NULL)
    		{
    			free(l->neighbor->eProp->prop[1]);
    			l= l->next;
    		}
    	}
    }
    destroyGraph(trustGraph);
}
