/*
 ============================================================================
 Name        : SD-2014-Part-1.c
 Version     :
 Copyright   : Lydia/Theodore/Kostas
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "GraphLib.h"
#include "Metrics.h"
#include "Query1.h"
#include "Query2.h"
#include "Query3.h"
#include "Query4.h"
#include "Part3.h"
#include "defines.h"

#define TRUST_GRAPH_REL_PROPERTIES_NUM 2

#define CHECK(text, actual, expected)                         \
if (actual != expected) {                                   \
printf("%-30s: Failed | actual = %3d, expected = %3d\n",  \
text,                                              \
actual,                                            \
expected);                                         \
} else {                                                    \
printf("%-30s: Success\n", text);                     \
}

#define CHECKINT(text, actual, expected)                         \
  if (actual != expected) {                                   \
    printf("%-30s: Failed | actual = %3d, expected = %3d\n",  \
           text,                                              \
           actual,                                            \
           expected);                                         \
  } else {                                                    \
    printf("%-30s: Success\n", text);                     \
  }

#define CHECKDOUBLE(text, actual, expected)	\
if ((actual > expected && actual-expected > 0.01) || (actual < expected && expected - actual > 0.01))	\
	  printf("%-30s: Failed | actual = %3f, expected = %3f\n",text,actual,expected);	\
else  \
  printf("%-30s: Success\n", text); \


/* Creates a node person and sets its properties */
Node* setPersonProperties(int id, char* name, char* surname, char* gender, int age);
/* Creates an edge between two persons and sets its properties */
Edge* setEdgeProperties(int startNodeID, int endNodeID, char* type, int weight);
/* Prints a person's properties */
void printPersonProperties(Node* n);
/* Prints an edge's properties */
void printEdgeProperties(int startID, Graph * g);

/*****************/
/* Main Function */
/*****************/

void testBetweennessCentrality(int bucketsNumber, int bucketSize) {
    //create small graph for testing betweenness Centrality
    Graph* gBetw = createGraph(bucketsNumber, bucketSize);

    Node* n1Betw = createPerson(1, NULL);
    Node* n2Betw = createPerson(2, NULL);
    Node* n3Betw = createPerson(3, NULL);
    Node* n4Betw = createPerson(4, NULL);
    Node* n5Betw = createPerson(5, NULL);

    insertNode(n1Betw, gBetw);
    insertNode(n2Betw, gBetw);
    insertNode(n3Betw, gBetw);
    insertNode(n4Betw, gBetw);
    insertNode(n5Betw, gBetw);

    /* Create edges and set properties */
    Edge* e1Betw = createEdge(1, 2, NULL);
    Edge* e2Betw = createEdge(2, 1, NULL);
    Edge* e3Betw = createEdge(2, 3, NULL);
    Edge* e4Betw = createEdge(2, 4, NULL);
    Edge* e5Betw = createEdge(3, 2, NULL);
    Edge* e6Betw = createEdge(3, 5, NULL);
    Edge* e7Betw = createEdge(4, 2, NULL);
    Edge* e8Betw = createEdge(4, 5, NULL);
    Edge* e9Betw = createEdge(5, 3, NULL);
    Edge* e10Betw = createEdge(5, 4, NULL);

    /* Insert edges in graph */
    insertEdge(1, e1Betw, gBetw);
    insertEdge(2, e2Betw, gBetw);
    insertEdge(2, e3Betw, gBetw);
    insertEdge(2, e4Betw, gBetw);
    insertEdge(3, e5Betw, gBetw);
    insertEdge(3, e6Betw, gBetw);
    insertEdge(4, e7Betw, gBetw);
    insertEdge(4, e8Betw, gBetw);
    insertEdge(5, e9Betw, gBetw);
    insertEdge(5, e10Betw, gBetw);

    double betwCentrty1 = betweennessCentrality(n1Betw, gBetw);
    CHECKDOUBLE("Small Graph betweenness centrality node:1 ", betwCentrty1, 0.0 / 6.0);

    double betwCentrty2 = betweennessCentrality(n2Betw, gBetw);
    CHECKDOUBLE("Small Graph betweenness centrality node:2 ", betwCentrty2, 3.5 / 6.0);

    double betwCentrty3 = betweennessCentrality(n3Betw, gBetw);
    CHECKDOUBLE("Small Graph betweenness centrality node:3 ", betwCentrty3, 1.0 / 6.0);

    double betwCentrty4 = betweennessCentrality(n4Betw, gBetw);
    CHECKDOUBLE("Small Graph betweenness centrality node:4 ", betwCentrty4, 1.0 / 6.0);

    double betwCentrty5 = betweennessCentrality(n5Betw, gBetw);
    CHECKDOUBLE("Small Graph betweenness centrality node:5 ", betwCentrty5, 0.5 / 6.0);
}

void testClosenessCentrality(int bucketsNumber, int bucketSize) {
    //create small graph for testing betweenness Centrality
    Graph* gClos = createGraph(bucketsNumber, bucketSize);

    Node* n1Clos = createPerson(1, NULL);
    Node* n2Clos = createPerson(2, NULL);
    Node* n3Clos = createPerson(3, NULL);
    Node* n4Clos = createPerson(4, NULL);
    Node* n5Clos = createPerson(5, NULL);
    Node* n6Clos = createPerson(6, NULL);
    Node* n7Clos = createPerson(7, NULL);

    insertNode(n1Clos, gClos);
    insertNode(n2Clos, gClos);
    insertNode(n3Clos, gClos);
    insertNode(n4Clos, gClos);
    insertNode(n5Clos, gClos);
    insertNode(n6Clos, gClos);
    insertNode(n7Clos, gClos);

    /* Create edges and set properties */
    Edge* e1Clos = createEdge(1, 2, NULL);
    Edge* e2Clos = createEdge(1, 3, NULL);
    Edge* e3Clos = createEdge(2, 1, NULL);
    Edge* e4Clos = createEdge(2, 3, NULL);
    Edge* e5Clos = createEdge(3, 1, NULL);
    Edge* e6Clos = createEdge(3, 2, NULL);
    Edge* e7Clos = createEdge(3, 4, NULL);
    Edge* e8Clos = createEdge(4, 3, NULL);
    Edge* e9Clos = createEdge(4, 5, NULL);
    Edge* e10Clos = createEdge(5, 4, NULL);
    Edge* e11Clos = createEdge(5, 6, NULL);
    Edge* e12Clos = createEdge(5, 7, NULL);
    Edge* e13Clos = createEdge(6, 5, NULL);
    Edge* e14Clos = createEdge(6, 7, NULL);
    Edge* e15Clos = createEdge(7, 5, NULL);
    Edge* e16Clos = createEdge(7, 6, NULL);


     /* Insert edges in graph */
    insertEdge(1, e1Clos, gClos);
    insertEdge(1, e2Clos, gClos);
    insertEdge(2, e3Clos, gClos);
    insertEdge(2, e4Clos, gClos);
    insertEdge(3, e5Clos, gClos);
    insertEdge(3, e6Clos, gClos);
    insertEdge(3, e7Clos, gClos);
    insertEdge(4, e8Clos, gClos);
    insertEdge(4, e9Clos, gClos);
    insertEdge(5, e10Clos, gClos);
    insertEdge(5, e11Clos, gClos);
    insertEdge(5, e12Clos, gClos);
    insertEdge(6, e13Clos, gClos);
    insertEdge(6, e14Clos, gClos);
    insertEdge(7, e15Clos, gClos);
    insertEdge(7, e16Clos, gClos);


    double closCentrty1 = closenessCentrality(n1Clos, gClos);
    CHECKDOUBLE("Small Graph closeness centrality node:1 ", closCentrty1, 3.33 / 6.0);

    double closCentrty2 = closenessCentrality(n2Clos, gClos);
    CHECKDOUBLE("Small Graph closeness centrality node:2 ", closCentrty2, 3.33 / 6.0);

    double closCentrty3 = closenessCentrality(n3Clos, gClos);
    CHECKDOUBLE("Small Graph closeness centrality node:3 ", closCentrty3, 4.16 / 6.0);

    double closCentrty4 = closenessCentrality(n4Clos, gClos);
    CHECKDOUBLE("Small Graph closeness centrality node:4 ", closCentrty4, 4.0 / 6.0);

    double closCentrty5 = closenessCentrality(n5Clos, gClos);
    CHECKDOUBLE("Small Graph closeness centrality node:5 ", closCentrty5, 4.16 / 6.0);

    double closCentrty6 = closenessCentrality(n6Clos, gClos);
    CHECKDOUBLE("Small Graph closeness centrality node:6 ", closCentrty6, 3.33 / 6.0);

    double closCentrty7 = closenessCentrality(n7Clos, gClos);
    CHECKDOUBLE("Small Graph closeness centrality node:7 ", closCentrty7, 3.33 / 6.0);

}


//Edge* setEdgeTrustProperties(int startNodeID, int endNodeID, double trust) {
//
//    /*create edge properties*/
//    Properties* propEdge = createProperties(TRUST_GRAPH_REL_PROPERTIES_NUM);
//    *(double *)propEdge->prop[1] = trust;
//
//    /*create an edge*/
//    Edge* e = createEdge(startNodeID, endNodeID, propEdge);
//    return e;
//}

void testTidalTrust(int bucketsNumber, int bucketSize) {
    //create small graph for testing tidal's trust algorithm result
    Graph* gtt = createGraph(bucketsNumber, bucketSize);

    Node* n1tt = setPersonProperties(1,NULL,NULL,NULL,10);
    Node* n2tt = setPersonProperties(2, NULL,NULL,NULL,10);
    Node* n3tt = setPersonProperties(3, NULL,NULL,NULL,10);
    Node* n4tt = setPersonProperties(4, NULL,NULL,NULL,10);
    Node* n5tt = setPersonProperties(5, NULL,NULL,NULL,10);
    Node* n6tt = setPersonProperties(6,NULL,NULL,NULL,10);
    Node* n7tt = setPersonProperties(7,NULL,NULL,NULL,10);
    Node* n8tt = setPersonProperties(8,NULL,NULL,NULL,10);
    Node* n9tt = setPersonProperties(9,NULL,NULL,NULL,10);
    Node* n10tt = setPersonProperties(10,NULL,NULL,NULL,10);
    Node* n11tt = setPersonProperties(11,NULL,NULL,NULL,10);

    insertNode(n1tt, gtt);
    insertNode(n2tt, gtt);
    insertNode(n3tt, gtt);
    insertNode(n4tt, gtt);
    insertNode(n5tt, gtt);
    insertNode(n6tt, gtt);
    insertNode(n7tt, gtt);
    insertNode(n8tt, gtt);
    insertNode(n9tt, gtt);
    insertNode(n10tt, gtt);
    insertNode(n11tt, gtt);


    Edge* e1tt = setEdgeProperties(1, 2,"knows", 1.0);
    Edge* e2tt = setEdgeProperties(1, 5,"knows", 1.0);
    Edge* e3tt = setEdgeProperties(2, 3,"knows", 0.9);
    Edge* e4tt = setEdgeProperties(2, 4,"knows", 0.9);
    Edge* e5tt = setEdgeProperties(3, 6,"knows", 0.8);
    Edge* e6tt = setEdgeProperties(4, 6,"knows", 0.3);
    Edge* e7tt = setEdgeProperties(4, 7,"knows", 0.9);
    Edge* e8tt = setEdgeProperties(5, 10,"knows", 0.9);
    Edge* e9tt = setEdgeProperties(6, 9,"knows", 1.0);
    Edge* e10tt = setEdgeProperties(7, 8,"knows", 1.0);
    Edge* e11tt = setEdgeProperties(8, 9,"knows", 1.0);
    Edge* e12tt = setEdgeProperties(9, 11,"knows", 1.0);
    Edge* e13tt = setEdgeProperties(10, 11,"knows", 0.4);

    /* Insert edges in graph */
    insertEdge(1, e1tt, gtt);
    insertEdge(1, e2tt, gtt);
    insertEdge(2, e3tt, gtt);
    insertEdge(2, e4tt, gtt);
    insertEdge(3, e5tt, gtt);
    insertEdge(4, e6tt, gtt);
    insertEdge(4, e7tt, gtt);
    insertEdge(5, e8tt, gtt);
    insertEdge(6, e9tt, gtt);
    insertEdge(7, e10tt, gtt);
    insertEdge(8, e11tt, gtt);
    insertEdge(9, e12tt, gtt);
    insertEdge(10, e13tt, gtt);

    Node *att = lookupNode(1, gtt);

    Node *btt = lookupNode(11, gtt);
    //Estimate trust(1,11)
    double trust1_11 = estimateTrust(att, btt, gtt);
    CHECKDOUBLE("Graph estimate trust (1,11)", trust1_11, 0.36);

    //Estimate trust(1,9)
    Node *ctt = lookupNode(9, gtt);
    double trust1_9 = estimateTrust(att, ctt, gtt);
    CHECKDOUBLE("Graph estimate trust (1,9)", trust1_9, 0.72);

}

int main(void) {
    
//    int i,j;
	clock_t start = clock(), diff;

    /*create empty graph*/
    Graph* g = createGraph(M, C);
    /*create node and set node properties*/
    insertPeople("person.csv","person_knows_person.csv",g);

//    addInterests("person_hasInterest_tag.csv",g);
//    double avgPthLgth = averagePathLength(g);
//    CHECKDOUBLE("Graph average path length", avgPthLgth, 5.0322);
//    int diam = diameter(g);
//    CHECKINT("Graph diameter", diam, 14);
//
//    int ccNumber = numberOfCCs(g);
//    CHECKINT("Graph number of components ", ccNumber, 1);
//
//    int maximumCC = maxCC(g);
//    CHECKINT("Graph maximum connected component ", maximumCC, 111);
//
//    double dense = density(g);
//    CHECKDOUBLE("Graph density ", dense, 0.0732);
//    int closenessIds[5] = {1734, 38, 8899, 3501, 75};
//    printf("Closeness Checking...\n");
//    float closenessIdsRes[5] = {0.3430,0.3439,0.3147,0.2667,0.1594};
//    for (i = 0; i < 5; ++i) {
//    	int nodeID = closenessIds[i];
//    	Node* node = lookupNode(nodeID, g);
//    	double closCentrty = closenessCentrality(node, g);
//    	CHECKDOUBLE("Graph closeness centrality ", closCentrty, closenessIdsRes[i]);
//    }
//    printf("Betweenness Checking...\n");
//    int betweennessIds[5] = {1734,   38,     8899,   9900,   75};
//    float betweennessIdsRes[5] = {  0.306,  0.053,  0.018,  0.005,  0.000};
//
//    for (i = 0; i < 5; ++i) {
//    	int nodeID = betweennessIds[i];
//    	Node* node = lookupNode(nodeID, g);
//    	double betwCentrty = betweennessCentrality(node, g);
//    	CHECKDOUBLE("Graph betweenness centrality ", betwCentrty, betweennessIdsRes[i]);
//    }
//    //graph queries calls
//    // Query 1 //
//    Matches* match;
//    Node *dateNode = lookupNode(3755, g);
//    int commonInterests = 1, ageDiff = 30, acquaintanceHops = 3, matchesNum = 1;
//    match = matchSuggestion(dateNode, commonInterests, acquaintanceHops, ageDiff, matchesNum, g);
//
//    //match result : 7107 - work_at_organization: 1650
//    //get first pair's person ids
//    for(i=0;i<matchesNum;i++)
//    	printf("Match Number %d with ID = %d and Score = %f\n",i,match[i].ID,match[i].score);
//
//    CHECKINT("Query 1: Date result 1st id", match[0].ID, 7107);
//    free(match);
//
//
//    // Query 2 //
//    //estimate stalkers graph with closeness centrality
//    Graph* stalkersGraphCloseCentr;
//    int stalkersNum = 7, likesNumber = 1, centralityMode = 1;
//    Stalkers stalkersCloseCentr[stalkersNum];
//    stalkersGraphCloseCentr = getTopStalkers(stalkersNum, likesNumber, centralityMode, g, stalkersCloseCentr);
//
//    int stalkersResultsIds[] = {347, 495, 7768, 8354, 8403, 8899, 9633};
//    int stalkerResultSize = 7;
//    int stalkerID;
//    int counter = 0;
//    int k;
//    for (i = 0; i < stalkersNum; ++i) {
//    	stalkerID = stalkersCloseCentr[i].ID;
//    	if (stalkerID != -1) {
//    		++counter;
//    	}
//    	for (k = 0; k < stalkerResultSize; ++k) {
//    		if (stalkersResultsIds[k] == stalkerID) {
//    			break;
//    		}
//    	}
//    	if (k == stalkerResultSize) {
//    		printf("You wrongly labeled person with id %d as Stalker\n", stalkerID);
//    	}
//    }
//    CHECKINT("Query 2: Stalker closeness results size", stalkerResultSize, counter);
//    for(i=0;i<stalkersNum;i++)
//    	printf("Stalker Number %d with ID = %d and Score = %f\n",i,stalkersCloseCentr[i].ID,stalkersCloseCentr[i].score);
//    //run metrics on stalker-graph
//    Metrics(stalkersGraphCloseCentr);
//
//    destroyStalkers(stalkersGraphCloseCentr);
//
//    //estimate stalkers graph with betweenness centrality */
//    Graph* stalkersGraphBetwCentr;
//    Stalkers stalkersBetwCentr[stalkersNum];
//    centralityMode = 2;
//    stalkersGraphBetwCentr = getTopStalkers(stalkersNum, likesNumber, centralityMode, g, stalkersBetwCentr);
//
//    counter = 0;
//    for (i = 0; i < stalkersNum; ++i) {
//    	stalkerID = stalkersBetwCentr[i].ID;
//    	if (stalkerID != -1) {
//    		++counter;
//    	}
//    	for (k = 0; k < stalkerResultSize; ++k) {
//    		if (stalkersResultsIds[k] == stalkerID) {
//    			break;
//    		}
//    	}
//    	if (k == stalkerResultSize) {
//    		printf("You wrongly labeled person with id %d as Stalker\n", stalkerID);
//    	}
//    }
//    CHECKINT("Query 2: Stalker betweenness results size", stalkerResultSize, counter);
//    for(i=0;i<stalkersNum;i++)
//        	printf("Stalker Number %d with ID = %d and Score = %f\n",i,stalkersBetwCentr[i].ID,stalkersBetwCentr[i].score);
//
//    //run metrics on stalker-graph
//    Metrics(stalkersGraphBetwCentr);
//    destroyStalkers(stalkersGraphBetwCentr);
//
//    // Query 3 //
//    int trendsNum = 4;
//    //allocate result tables before calling query and pass them as parameters
//    char** womenTrends;
//    womenTrends = (char**) malloc(trendsNum * sizeof (char*));
//    char** menTrends;
//    menTrends = (char**) malloc(trendsNum * sizeof (char*));
//    findTrends(trendsNum, g, womenTrends, menTrends);
//
//    printf("Top Women - Men Trends\n");
//    char* menTrendsResults[4] = {"Sun_Yat-sen", "Constantine_the_Great",
//    		"Sigmund_Freud", "Hussein_of_Jordan"}; //IDS: {417,11622,468,1398}
//    char* womenTrendsResults[4] = {"Adolf_Hitler", "Chiang_Kai-shek", "Gil_Kane","John_Locke"}; //IDS: {138,416,null,null}
//
//    int counterW = 0, counterM = 0;
//
//    for (j = 0; j < 4; ++j) {
//    	if (menTrends[j] != NULL) {
//    		++counterM;
//
//    		for (k = 0; k < 4; ++k) {
//
//    			if (strcmp(menTrends[j], menTrendsResults[k]) == 0) {
//    				break;
//    			}
//    		}
//    		if (k == 4) {
//    			printf("You wrongly labeled tag with %s as top trend\n", menTrends[j]);
//    		}
//    	}
//    }
//    CHECKINT("Query 3: Mens Trends result size", 4, counterM);
//
//
//    for (j = 0; j < 4; ++j) {
//    	if (womenTrends[j] != NULL) {
//    		++counterW;
//
//    		for (k = 0; k < 4; ++k) {
//
//    			if (strcmp(womenTrends[j], womenTrendsResults[k]) == 0) {
//    				break;
//    			}
//    		}
//    		if (k == 4) {
//    			printf("You wrongly labeled tag with %s as top trend\n", womenTrends[j]);
//    		}
//    	}
//    }
//
//
//
//    CHECKINT("Query 3: Women Trends result size", 4, counterW);
//    for(i=0;i<4;i++)
//    {
//        free(womenTrends[i]);
//        free(menTrends[i]);
//    }
//    free(womenTrends);
//    free(menTrends);
//
//
//    // Query 4 //
//    Graph* trustGraph;
//    int forumID = 34680;
//
//    trustGraph = buildTrustGraph(forumID, g);
//
//    int trustANodeId = 30;
//    int trustBNodeId = 9805;
//    int trustCNodeId = 9700;
//    Node *ta = lookupNode(trustANodeId, trustGraph);
//    Node *tb = lookupNode(trustBNodeId, trustGraph);
//    Node *tc = lookupNode(trustCNodeId, trustGraph);
//    double trustAB;
//    trustAB = estimateTrust(ta, tb, trustGraph);
//    printf("Trust between nodes (%d,%d) is %f\n", trustANodeId, trustBNodeId, trustAB);
//    CHECKDOUBLE("Trust: (30,9805) ", trustAB, 0.134);
//
//    double trustAC;
//    trustAC = estimateTrust(ta, tc, trustGraph);
//    printf("Trust between nodes (%d,%d) is %f\n", trustANodeId, trustCNodeId, trustAC);
//    CHECKDOUBLE("Trust: (30,9700) ", trustAC, 0.15);
//    destroyTrustGraph(trustGraph);
//    destroyGraph(g);
    int N = 5;
    findTopN(g, N);
    destroyGraph(g);
    diff = clock() - start;
    double total_time = ((double)diff)/ CLOCKS_PER_SEC ;
    printf("Total tme: %.4f seconds\n",total_time);
    return EXIT_SUCCESS;
}

Node* setPersonProperties(int id, char* name, char* surname, char* gender, int age) {

    /*create properties*/
    Properties* prop = createProperties(PERSON_PROPERTIES_NUM);
    setStringProperty(name, 0, prop);
    setStringProperty(surname, 1, prop);
    setStringProperty(gender, 2, prop);
    setIntegerProperty(age, 3, prop);

    /*create node*/
    Node* n = createPerson(id, prop);

    return n;
}

Edge* setEdgeProperties(int startNodeID, int endNodeID, char* type, int weight) {

    /*create edge properties*/
    Properties* propEdge = createProperties(PERSON_REL_PROPERTIES_NUM);
    setStringProperty(type, 0, propEdge);
    setIntegerProperty(weight, 1, propEdge);

    /*create an edge*/
    Edge* e = createEdge(startNodeID, endNodeID, propEdge);
    return e;
}

void printPersonProperties(Node* n) {
	Person *p = n->obj;
    printf("Person with ID %d and Name %s, Surname %s, Gender %s and Age %ld\n",n->ID,(char *)p->nProp->prop[0],(char *)p->nProp->prop[1],(char *)p->nProp->prop[2],(long int)p->nProp->prop[3]);
}

void printEdgeProperties(int id, Graph * g) {
    Node * n = lookupNode(id,g);
    Person * p = n->obj;
    List * tmp = p->list;
    printf(" --------\n");
    printf("| ID = %d |\n",n->ID);
    printf(" --------\n");
    while(tmp != NULL)
    {
        printf("+%d %s %d\n",n->ID,(char *)tmp->neighbor->eProp->prop[0],tmp->neighbor->ID);
        tmp = tmp->next;
    }
    printf("\n");
}
