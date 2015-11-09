/*
 * GraphLib.h
 *
  */

#ifndef GRAPHLIB_H_
#define GRAPHLIB_H_

/*
 * Queue
 */
struct Queue {
    int ID;
    struct Queue * next;
};
typedef struct Queue Queue;

struct properties {
    void ** prop;
};
typedef struct properties Properties;


/**
 * Pair of integers
 */
struct pair {
    int ID; // the id of a node
    int distance; //the distance from the node
};
typedef struct pair Pair;

/**
 * Edge
 */
struct edge {
    int ID;
    Properties * eProp;
};
typedef struct edge Edge;

typedef struct List{
    Edge * neighbor;
    struct List * next;
}List;

/**
 * Node
 */
struct node{
	int ID;
	void * obj;
};
typedef struct node Node;

struct person {
    int distN,edges,prop_num,inter_num;
    Pair * pair;
    int  ** interests;
    Properties * nProp;
    List * list;
};
typedef struct person Person;

/*
 * Graph
 */
struct graph {
    int hash,h_pointer,c,size,nodes_sum;
    int * position,* cell_size;
    Node *** table;
};
typedef struct graph Graph;

/**
* ResultQueue
*/
struct resultQueue {
	Pair * pair;
    struct resultQueue * next;
};
typedef struct resultQueue ResultQueue;

/**
* ResultSet
*/
struct resultSet {
	Graph * g;
    ResultQueue * start,* queue, * end;
};
typedef struct resultSet ResultSet;

/*Functions for implementation of part 1*/
Graph* createGraph(int m, int c);
int destroyGraph(Graph* g);
int insertNode(const Node* const n, Graph* g);
int insertEdge(int startID, const Edge* const e, Graph* g);
Node* lookupNode(int ID, const Graph* const g);
int reachNode1(int startID, int endID, const Graph*  const g);
ResultSet* reachNodeN(int startID, const Graph*  const g);
int next(ResultSet* resultSet, Pair* pair);

/*********************************Structs part 2*********************************************************/



/*Functions for implementation of part 2*/

/*Help functions for test main*/
void insertPeople(char * file,char * file2,Graph * g);
Edge* setEdgeProperties(int startNodeID, int endNodeID, char* type, int weight);
Node* setPersonProperties(int id, char* name, char* surname, char* gender, int age);
Properties* createProperties(int number);
void setStringProperty(char* property, int index, Properties* p);
void setIntegerProperty(int property, int index, Properties* p);
char* getStringProperty(int index, Properties* p);
int getIntegerProperty(int index, Properties* p);
Node* createPerson(int ID, Properties* p);
Edge* createEdge(int startID, int endID, Properties* p);
void printSet(ResultSet * set);
void printTable(Graph * g);
void freeList(Queue * start,int ID, const Graph* const g);
int ** insertionT(int ** arr,int k);
int binary_search(int array[],int size, int interID);

#endif /* GRAPHLIB_H_ */
