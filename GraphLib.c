/*
 * GraphLib.c
 *
 */

#include <stdio.h>

#include "GraphLib.h"
#include "defines.h"

/**
 * Creates a properties object
 * number: the number of properties
 * return value: a properties object
 */
Properties* createProperties(int number) {
	Properties * p = malloc(sizeof(Properties));
	if((p->prop = malloc(number*sizeof(void *)))==NULL)
	{
		perror("Malloc on createProperties failed! ");
		exit(-1);
	}
	return p;
}

/**
 * Sets a string property "property" in position "index"
 * property: the type of the property
 * index: the position of the property
 * p: the properties object
 */
void setStringProperty(char* property, int index, Properties* p) {
	p->prop[index] = (void *)property;
}

/**
 * Sets an integer property "property" in position "index"
 * property: the value of the property
 * index: the position of the property
 * p: the properties object
 */
void setIntegerProperty(int property, int index, Properties* p) {
	long int tmp = (long int) property;
	p->prop[index] = (void *) tmp;
}

void printTable(Graph * g)//node id printing in every bucket
{
	Node *** table = g->table;
	int i,j;
	printf("hash is %d\n",g->hash);
	for (i = 0;i<g->size;i++)
	{
		if (i != g->h_pointer)
			printf("   ");
		else
			printf("-> ");
		printf("%do bucket with (%d) elements & cell_size %d: ",i+1,g->position[i],g->cell_size[i]);
		for (j = 0;j<g->position[i];j++)
			printf("%d ",table[i][j]->ID);
		printf("\n");
	}
	printf("\n");
	printf("Graph with %d records\n",g->nodes_sum);
}
//
/**
 * Creates a node with specific properties
 * id: the id of the node
 * p: the properties of the node
 * return value: the requested node
 */
Node* createPerson(int id, Properties* p) {
	Node * n = malloc(sizeof(Node));
	Person * person;
	if((person = malloc(sizeof(Person)))==NULL)
	{
		perror("The first malloc on createPerson failed! ");
		exit(-1);
	}
	if ((person->pair = malloc(sizeof(Pair)))==NULL)
	{
		perror("The second malloc on createPerson failed! ");
		exit(-1);
	}
	person->pair->distance = -1;
	person->edges = 0;
	person->prop_num = PERSON_PROPERTIES_NUM;
	person->pair->ID = -1;
	person->distN = -1;
	person->inter_num = 0;
	n->ID = id;
	person->nProp = p;
	person->interests = NULL;
	person->list = NULL;
	n->obj = person;
	return n;
}

/**
 * Creates an edge with specific properties
 * startID: the id of the start node
 * endID: the id of the end node
 * p: the properties of the edge
 * return value: the requested edge
 */
Edge* createEdge(int startID, int endID, Properties* p) {
	Edge * e = malloc(sizeof(Edge));
	e->eProp = p;
	e->ID = endID;
	return e;
}


///************************************/
///* Implement for part 1 */
///************************************/
//
///* Creates an empty graph
// * m: size of hashtable
// * c: size of cells in a bucket
// * return value: an empty graph*/
Graph* createGraph(int m, int c) {
	int i;
	Graph * g;
	if ((g = malloc(sizeof(Graph)))==NULL) //graph creation
	{
		perror("The first malloc on createGraph failed! ");
		exit(-1);
	}
	if ((g->table = malloc(m*sizeof(Node**)))==NULL) //hashtable creation
	{
		perror("The second malloc on createGraph failed! ");
		exit(-1);
	}
	for (i = 0;i < m;i++)
	{
		if ((g->table[i] = malloc(c*sizeof(Node*)))==NULL) //bucket creation
		{
			perror("The third(a) malloc on createGraph failed! ");
			exit(-1);
		}
	}
	if ((g->position = malloc(m*sizeof(int)))==NULL)
	{
		perror("The fourth malloc on createGraph failed! ");
		exit(-1);
	}
	if ((g->cell_size = malloc(m*sizeof(int)))==NULL)
	{
		perror("The fourth malloc on createGraph failed! ");
		exit(-1);
	}
	for (i= 0;i < m;i++)
	{
		g->position[i] = 0;
		g->cell_size[i] = c;
	}
	g->hash = m;
	g->c = c;
	g->h_pointer = 0;
	g->size = m;
	g->nodes_sum = 0;
	return g;
}

void insertPeople(char * file,char * file2,Graph * g)
{
	FILE * fp = fopen(file,"r");
	int ID,ID2,age;
	char * name, * surname, * gender, buffer[200],*s;
	Node * n;
	Edge * e;
	fgets(buffer,200,fp);
	int i = 0;
	while(fgets(buffer,200,fp)!= NULL)
	{
		i++;
		ID = atoi(strtok (buffer,"|"));
		s = strtok (NULL,"|");
		name = malloc(strlen(s)+1);
		strcpy(name,s);
		s = strtok (NULL,"|");
		surname = malloc(strlen(s)+1);
		strcpy(surname,s);
		s = strtok (NULL,"|");
		gender = malloc(strlen(s)+1);
		strcpy(gender,s);
		age = 2014 - atoi(strtok (NULL,"-"));
		n = setPersonProperties(ID,name,surname,gender,age);
		insertNode(n,g);
	}
	fclose(fp);
	fp = fopen(file2,"r");
	fgets(buffer,200,fp);
	while(fgets(buffer,200,fp)!= NULL)
	{
		ID = atoi(strtok (buffer,"|"));
		ID2 = atoi(strtok (NULL,"\n"));
		e = setEdgeProperties(ID,ID2,"knows",0);
		insertEdge(ID,e,g);
	}
	fclose(fp);
}

/* Destroys a graph
 * g: the graph
 * return value: success/fail status */
int destroyGraph(Graph* g) {
	int i,j,k;
	List * tmp;
	Person * p;
	int b = 0;
	for (i=0;i<g->size;i++)
	{
		for(j=0;j<g->position[i];j++)
		{
			b++;
			p = g->table[i][j]->obj;
			while(p->list != NULL) //adjacency list deallocation
			{
				tmp = p->list;
				p->list = p->list->next;
				if(tmp->neighbor->eProp != NULL)
				{
					free(tmp->neighbor->eProp->prop);
					free(tmp->neighbor->eProp); //edge deallocation
				}
				free(tmp->neighbor);
				free(tmp);
			}
			if( p->inter_num != 0 )
			{
				free(p->interests[0]);
				free(p->interests[1]);
				free(p->interests);
			}
			if(p->nProp != NULL)
			{
				for(k=0; k<p->prop_num-1; k++)
					free(p->nProp->prop[k]);
				free(p->nProp->prop);
				free(p->nProp);
			}
			free(p->pair);
			free(p); //node deallocation
			free(g->table[i][j]);
		}
		free(g->table[i]);
	}
	free(g->table); //hash table deallocation
	free(g->cell_size);
	free(g->position);
	free(g); //graph deallocation
	return 1;
}

/* Inserts a node in the graph
 * n: the node to insert
 * g: the graph
 * return value: success/fail status */
int insertNode(const Node* const n, Graph* g) {

	int index,bucket_index,result;
	index = n->ID%g->hash;
	g->nodes_sum++;  //increasing number of nodes
	if (g->h_pointer > index) //Before the pointer of the hash table
		index = n->ID%(2*g->hash);
	bucket_index =  g->position[index];
	//overflow//
	if (bucket_index == g->cell_size[index] && bucket_index != 0)
	{
		int i = 0,noe=0;
		// bucket split
		g->size++;
		//positions array increase
		if ((g->position = realloc(g->position,g->size*sizeof(int)))==NULL)
		{
			perror("The first realloc on insertNode failed! ");
			exit(-1);
		}
		if ((g->cell_size = realloc(g->cell_size,g->size*sizeof(int)))==NULL)
		{
			perror("The first realloc on insertNode failed! ");
			exit(-1);
		}
		g->position[g->size-1] = 0;
		g->cell_size[g->size-1] = g->c;
		//hashtable increase
		if ((g->table = realloc(g->table,g->size*sizeof(Node**)))==NULL)
		{
			perror("The second realloc on insertNode failed! ");
			exit(-1);
		}
		//New bucket allocation
		if ((g->table[g->size-1] = malloc(g->c*sizeof(Node*)))==NULL)
		{
			perror("The first malloc on insertNode failed! ");
			exit(-1);
		}
		//Division of the elements 
		noe =  g->position[g->h_pointer]; //number of elements in the splitted bucket
		g->position[g->h_pointer] = 0;
		while(i<noe)
		{
			result = g->table[g->h_pointer][i]->ID%(g->hash*2); //result of the second hash
			if (result == g->h_pointer) //check if the element stays in the old bucket
			{
				g->table[result][g->position[g->h_pointer]] = g->table[result][i];
				g->position[g->h_pointer]++;
			}
			else{
				if (g->position[result] == g->cell_size[result]-1) //bucket overflow -> double it's size
				{
					g->cell_size[result] *= 2;
					if ((g->table[result] = realloc(g->table[result],g->cell_size[result]*sizeof(Node*)))==NULL)
					{
						perror("The third realloc on insertNode failed! ");
						exit(-1);
					}
				}
				g->table[result][g->position[result]] = g->table[g->h_pointer][i];
				g->position[result]++;
			}
			i++;
		}
		if (g->h_pointer == index)
			index = n->ID%(2*g->hash);
		if (g->h_pointer != index || (g->position[g->h_pointer] == g->cell_size[index] && n->ID%(2*g->hash)==g->h_pointer)) 
		{
			g->cell_size[index] *= 2;
			if ((g->table[index] = realloc(g->table[index],g->cell_size[index]*sizeof(Node*)))==NULL) // double the size of the overflow bucket
			{
				perror("The third realloc on insertNode failed! ");
				exit(-1);
			}
		}
		//checking for "round"
		if (g->hash-1 == g->h_pointer)
		{
			g->hash*=2;
			g->h_pointer = 0;
		}
		else
			g->h_pointer++;
	}
	//insertion of the element 
	int num_of_elements;
	if (g->position[index] == 0)
	{
		g->position[index]++;
		g->table[index][0] = (Node *) n;
		return 1;
	}
	bucket_index = 0;
	while (1)
	{
		if (bucket_index < g->position[index] && n->ID > g->table[index][bucket_index]->ID)
			bucket_index++;
		else
		{
			if(g->position[index] != bucket_index){
				num_of_elements = g->position[index] - bucket_index;
				memmove(&(g->table[index][bucket_index+1]), &(g->table[index][bucket_index]),num_of_elements*sizeof(Node *));
				g->table[index][bucket_index] = (Node *) n;
			}
			else
				g->table[index][bucket_index] = (Node *) n;
			g->position[index]++;
			break;
		}
	}
	return 1;
}

/* Inserts an edge in the graph
 * startID: the id of the start node
 * e: the edge we want to add to the start node
 * g: the graph
 * return value: success/fail status */
int insertEdge(int startID, const Edge* const e, Graph* g) {
	Node * node = lookupNode(startID,g);
	if (node == NULL)
		return 0;
	Person *p = node->obj;
	p->edges++;
	if (p->list == NULL)
	{
		p->list = malloc(sizeof(List));
		p->list->neighbor = (Edge *) e;
		p->list->next = NULL;
	}
	else
	{
		List * point = p->list,* previous = NULL,*tmp=NULL;
		while(point != NULL)
		{
			if(point->neighbor->ID > e->ID)
			{
				tmp = malloc(sizeof(List));
				tmp->neighbor = (Edge *) e;
				tmp->next = point;
				if (previous != NULL)
					previous->next = tmp;
				else
					p->list = tmp;
				break;
			}
			else if (point->next == NULL)
			{
				point->next = malloc(sizeof(List));
				point->next->neighbor = (Edge *) e;
				point->next->next = NULL;
				break;
			}
			else
			{
				previous = point;
				point = point->next;
			}
		}
	}
	return 1;
}

/* Retrieves a node from the graph
 * nodeID: the id of node to retrieve
 * g: the graph
 * return value: the requested node */
Node* lookupNode(int nodeID, const Graph* const g) {
	int index = nodeID%g->hash,start,end,mid;
	if (g->h_pointer > index) //If our element is before the hash_pointer
		index = nodeID%(2*g->hash);
	start = 0;
	end = g->position[index]-1;
	while(start<=end)
	{
		mid = (start+end)/2;
		if (g->table[index][mid]->ID == nodeID)
			return g->table[index][mid];
		else if (g->table[index][mid]->ID < nodeID)
			start = mid + 1;
		else
			end = mid - 1;
	}
	return NULL;
}

/* Bidirectional BFS starting point*/
Queue * initQueue(int startID,int endID,int * res,Queue ** q,List * l,Node * n,const Graph* const g)
{
	Queue * tmp;
	n = lookupNode(l->neighbor->ID,g);
	Person *p = n->obj;
	if(p->pair->ID == endID) { //common neighbor -> distance = 2
		*res = 2;
		if(n->ID == endID) //if startID is neighbor of endID -> distance = 1
			*res = 1;
		return NULL;
	}
	*q = malloc(sizeof(Queue)); //q: start of every queue
	(*q)->ID = l->neighbor->ID;
	p->pair->ID = startID;
	p->pair->distance = 1;
	(*q)->next = NULL;
	l = l->next;
	tmp = *q;
	while(l != NULL) //addition of the startID's neighbors in the queue
	{
		tmp->next = malloc(sizeof(Queue));
		tmp->next->ID = l->neighbor->ID;
		tmp->next->next = NULL;
		n = lookupNode(l->neighbor->ID,g);
		p = n->obj;
		if(p->pair->ID == endID){
			*res = 2;
			if(n->ID == endID)
				*res = 1;
			return NULL;
		}
		p->pair->ID = startID;
		p->pair->distance = 1;
		l = l->next;
		tmp = tmp->next;
	}
	return tmp;
}

int result(int startID, int endID, Queue * q, Queue ** end,const Graph* const g){
	Node * n;
	Person *p,* ptmp;
	int end_id = (*end)->ID;
	List * l;
	while(1)
	{
		n = lookupNode(q->ID,g);
		p = n->obj;
		l = p->list;
		while(l != NULL) //Adding neighbors of n (frontier creation)
		{
			n = lookupNode(l->neighbor->ID, g);
			ptmp = n->obj;
			if (ptmp->pair->ID == startID ) //if the current node is the node with id=startID is already in our list,continue
			{
				l = l->next;
				continue;
			}
			else if (ptmp->pair->ID == endID)//if the current node is the node with id=endID is already in our list (BFS END)
				return p->pair->distance+ptmp->pair->distance+1; //return sum of distances
			ptmp->pair->ID = startID;
			ptmp->pair->distance = p->pair->distance+1;
			(*end)->next = malloc(sizeof(Queue));
			(*end)->next->ID = l->neighbor->ID;
			(*end)->next->next = NULL;
			(*end) = (*end)->next;
			l = l->next;
		}
		if(q->ID == end_id)
			break;
		q = q->next;
	}
	return 0;
}

void freeList(Queue * start,int ID, const Graph* const g){ //deallocation of resources used in BFS

	Node * ntmp;
	Person * ptmp;
	Queue * tmp;
	while(start != NULL){ //list deallocation and pair->id restoration
		ntmp = lookupNode(start->ID,g);
		ptmp = ntmp->obj;
		ptmp->pair->ID = -1;
		tmp = start;
		start = start->next;
		free(tmp);
	}
	//pair->id of start ID restoration
	ntmp = lookupNode(ID,g);
	ptmp = ntmp->obj;
	ptmp->pair->ID = -1;
}

/* Finds shortest path distance between two nodes in an undirected graph with no edge weights
 * startID: the id of the start node
 * endID: the id of the end node
 * g: the graph
 * return value: the shortest path */
int reachNode1(int startID, int endID, const Graph* const g) {

	int found = 0, res;
	Node * n,* n2;
	List * l = NULL,*l2 = NULL;
	Queue * q = NULL, *q2 = NULL, * start = NULL, *end = NULL, * start2 = NULL, *end2 = NULL;
	if (startID == endID)
		return 0;
	Person *p,*p2;
	n2 = lookupNode(endID, g);
	p2 = n2->obj;
	n = lookupNode(startID, g);
	p = n->obj;
	l = p->list;
	l2 = p2->list;
	if(l == NULL || l2 == NULL)   //if at least one of the nodes has no neighbors,then their distance is infinite
		return INFINITY_REACH_NODE;
	p->pair->ID = n->ID;
	p2->pair->ID = n2->ID;
	if((end = initQueue(startID,endID,&res,&q,l,n,g)) == NULL || (end2 = initQueue(endID,startID,&res,&q2,l2,n2,g)) == NULL){
		start = q;
		start2 = q2;
		freeList(start,startID, g);
		freeList(start2,endID, g);
		return res;
	}
	start = q;
	start2 = q2;
	while(q != NULL && q2 != NULL)
	{
		//calling function result from both ends (bidirectional BFS)
		found = result(startID,endID,q,&end,g);
		if (found){
			freeList(start,startID, g);
			freeList(start2,endID, g);
			return found;
		}
		found = result(endID,startID,q2,&end2,g);
		if (found){
			freeList(start,startID, g);
			freeList(start2,endID, g);
			return found;
		}
		if(q == NULL || q2 == NULL){ //if at least one list is empty,then the nodes are not on the same connected component,so return inf
			freeList(start,startID, g);
			freeList(start2,endID, g);
			return INFINITY_REACH_NODE;
		}
		q = q->next;
		q2 = q2->next;
	}
	freeList(start,startID, g);
	freeList(start2,endID, g);
	return INFINITY_REACH_NODE;
}

void printSet(ResultSet * set) 
{
	ResultQueue * tmp = set->queue;
	while(tmp != NULL)
	{
		printf("ID: %d, Distance: %d\n",tmp->pair->ID,tmp->pair->distance);
		tmp = tmp->next;
	}
	printf("\n");
}

void clearPairs(ResultSet * set, Graph* g) //restoration of distN and deallocation of struct ResultSet
{
	int i,j;
	Person *p;
	for (i=0;i<g->size;i++)
		for (j=0;j< g->position[i];j++)
		{
			p = g->table[i][j]->obj;
			p->distN = -1;
		}
	free(set);
}

/* Finds shortest path distance between all reachable nodes from a given node
 * in an undirected graph with no edge weights
 * startID: the id of the start node
 * g: the graph
 * return value: a resultSet */
ResultSet* reachNodeN(int startID, const Graph* const g) { //set initialization
	ResultSet * set;
	ResultQueue *tmp;
	Node * n = lookupNode(startID,g);
	Person * p = n->obj;
	List *l = p->list;
	if (l == NULL)
		return NULL;
	p->distN = 0;
	set = malloc(sizeof(ResultSet));
	set->g = (Graph *)g;
	n = lookupNode(l->neighbor->ID,g);
	p = n->obj;
	p->distN = 1;
	set->queue = malloc(sizeof(ResultQueue)); //insertion of the first neighbor
	set->queue->pair = malloc(sizeof(Pair));
	set->queue->pair->ID = l->neighbor->ID;
	set->queue->pair->distance  = 1;
	set->queue->next = NULL;
	l=l->next;
	tmp = set->queue;
	set->start = tmp;
	while(l != NULL) //rest of the neighbors
	{
		n = lookupNode(l->neighbor->ID,g);
		p = n->obj;
		p->distN = 1;
		tmp->next = malloc(sizeof(ResultQueue));
		tmp->next->pair = malloc(sizeof(Pair));
		tmp->next->pair->ID = l->neighbor->ID;
		tmp->next->pair->distance = 1;
		tmp->next->next = NULL;
		tmp = tmp->next;
		l = l->next;
	}
	set->end = tmp;
	n->obj = p;
	return set;
}
/* Checks if another result is available
 * resultSet: the ResultSet returned by a reachNodeN call
 * pair: a Pair instance to hold the next result, the space for the Pair is
 * allocated by the caller
 * return value: 0 when no more record is available, 1 when it is
 */
int next(ResultSet* resultSet, Pair* pair) {
	ResultQueue * q;
	Node * n;
	Person * p;
	List * l;
	int d;
	if(resultSet == NULL)
		return 0;
	q = resultSet->queue;
	if(q == NULL)
	{
		clearPairs(resultSet,resultSet->g);
		return 0;
	}
	*pair = *(q->pair);
	n = lookupNode(pair->ID,resultSet->g);
	p = n->obj;
	l = p->list;
	d = p->distN;
	while(l != NULL)
	{
		n = lookupNode(l->neighbor->ID,resultSet->g);
		p = n->obj;
		if (p->distN != -1)
		{
			l = l->next;
			continue;
		}
		p->distN = d+1;
		resultSet->end->next = malloc(sizeof(ResultQueue));
		resultSet->end->next->pair = malloc(sizeof(Pair));
		resultSet->end->next->pair->ID = l->neighbor->ID;
		resultSet->end->next->pair->distance = d+1;
		resultSet->end->next->next = NULL;
		resultSet->end = resultSet->end->next;
		l = l->next;
	}
	resultSet->queue = resultSet->queue->next;
	free(q->pair); //delete pair from queue
	free(q);
	return 1;
}

int binary_search(int array[],int size, int interID)    //binary search of interID
{
	int start = 0,end = size-1, mid;
	while(start<=end)
	{
		mid = (start+end)/2;
		if (array[mid] == interID)
			return mid;
		else if (array[mid] < interID)
			start = mid + 1;
		else
			end = mid - 1;
	}
	return -1;
}
