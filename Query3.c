#include "Query3.h"

void updateQueue(int interID, int team, int new_member, Graph * interGraph)
{
	Node * n = lookupNode(interID,interGraph);
	InterObj * obj = n->obj;
	InterList * list = obj->team;
	Queue * tmp;
	while(list != NULL)
	{
		if(list->iNode->tag == team)    //finding proper team
		{
			list->iNode->noe++;
			tmp = malloc(sizeof(Queue));
			tmp->ID = new_member;
			tmp->next = list->iNode->q;
			list->iNode->q = tmp;
		}
		list = list->next;
	}
}

void printInterestGraph(Graph * interGraph) 
{
    int i,j;
    Node * n;
    InterObj * obj;
    InterList * team;
    Queue * q;
    for(i=0;i<interGraph->size;i++)
    {
        for(j=0;j<interGraph->position[i];j++)
        {
            n = lookupNode(interGraph->table[i][j]->ID,interGraph);
            obj = n->obj;
            printf("\nInterest %d\n",interGraph->table[i][j]->ID);
            team = obj->team;
            while(team != NULL) // printing teams
            {
                printf("Team %d: ",team->iNode->tag);
                q = team->iNode->q;
                while(q != NULL)    //printing members of every team
                {
                    printf("%d ",q->ID);
                    q = q->next;
                }
                printf("\n");
                team = team->next;
            }
        }
    }
    printf("\n");
}

int ** insertionT(int ** arr,int k) //sorting trends
{
    int i,tmp,swap;
    for (i = 1 ; i < k; i++) {
      tmp = i;
      while (tmp > 0 && arr[1][tmp] < arr[1][tmp-1]) {
        swap = arr[1][tmp];
        arr[1][tmp] = arr[1][tmp-1];
        arr[1][tmp-1] = swap;
        swap = arr[0][tmp];
        arr[0][tmp] = arr[0][tmp-1];
        arr[0][tmp-1] = swap;
        tmp--;
      }
    }
    return arr;
}

void groupInterests(Graph * g,Graph * interGraph,Node * n,int mode)
{
	int i,j, c1, c2, sum1,sum2, index;
	Person *p1, *p = n->obj, * ptmp = NULL;
	Node *n1, * inter = NULL;
	p->distN = 1;
	List * l;
	Queue *q = NULL, * end = NULL, * start = NULL, * tmp = NULL; //teams list
	InterObj * obj;
	InterList * team,*prev1,*prev2;
	Interests * inter1, * inter2, *swap;
	for(i = 0; i < p->inter_num; i++)   //inserting interests
	{
		if ((n1 = lookupNode(p->interests[0][i],interGraph)) == NULL)
		{
			p->interests[1][i] = 1;
			obj = malloc(sizeof(InterObj));
			obj->mmax = 0;  
			obj->wmax = 0;
			obj->next_num = 2; //next team number
			obj->team = malloc(sizeof(InterList)); //interest team list
			obj->team->next = NULL;
			obj->team->iNode = malloc(sizeof(Interests)); //team node
			obj->team->iNode->tag = 1; //team number
			obj->team->iNode->noe = 1; //team number of members
			obj->team->iNode->q = malloc(sizeof(Queue)); 
			obj->team->iNode->q->ID = n->ID; //member ID
			obj->team->iNode->q->next = NULL;
			inter = malloc(sizeof(Node));
			inter->ID = p->interests[0][i]; 
			inter->obj = obj;
			insertNode(inter,interGraph);
		}
		else
		{
			obj = n1->obj;
			p->interests[1][i] = 1;
			team = malloc(sizeof(InterList)); 
			team->next = NULL;
			team->iNode = malloc(sizeof(Interests)); 
			team->iNode->tag = 1; 
			team->iNode->noe = 1; 
			team->iNode->q = malloc(sizeof(Queue)); 
			team->iNode->q->ID = n->ID; 
			team->iNode->q->next = NULL;
			obj->next_num = 2;  
			obj->team = team;
		}
	}
	end = malloc(sizeof(Queue));
	end->ID = n->ID;
	end->next = NULL;
	q = start = end;
	while(q != NULL)
	{
		n = lookupNode(q->ID,g);
		p = n->obj;
		l = p->list;
		while(l != NULL)
		{
			n1 = lookupNode(l->neighbor->ID,g);
			p1 = n1->obj;
			if(!strcmp(p->nProp->prop[2],p1->nProp->prop[2])) //same gender
			{
				c1 = c2 = 0;
				if((sum1 = p->inter_num) == 0)
					break;
				if((sum2 = p1->inter_num) == 0)
				{
					l = l->next;
					continue;
				}
				while(c2<sum2)
				{
					n = lookupNode(q->ID,g);
					if (p->interests[0][c1] == p1->interests[0][c2]) //common interest
					{
						if(p1->interests[1][c2] == 0) //node not found
						{
							p1->interests[1][c2] = p->interests[1][c1]; 
							updateQueue(p1->interests[0][c2],p->interests[1][c1],n1->ID,interGraph);
						}
						else if(p->interests[1][c1] == p1->interests[1][c2]) //same team with parent node
						{
							if(c1 < sum1-1)
								c1++;
							c2++;
							continue;
						}
						else //node - parent node have different interest team
						{
							inter = lookupNode(p1->interests[0][c2],interGraph); //common interest in interestGraph
							obj = inter->obj;
							team = obj->team;
							swap = inter1 = inter2 = NULL;
							prev1 = prev2 = NULL;
							while(inter1 == NULL || inter2 == NULL) 
							{
								if (inter1 == NULL)
								{
									if(team->iNode->tag == p->interests[1][c1])
										inter1 = team->iNode;
									else
										prev1 = team;
								}
								if (inter2 == NULL){
									if(team->iNode->tag == p1->interests[1][c2])
										inter2 = team->iNode;
									else
										prev2 = team;
								}
								team = team->next;
							}
							if(inter1->noe < inter2->noe)
							{
								swap = inter1;
								inter1 = inter2;
								inter2 = swap;
								prev2 = prev1;
							}
							inter1->noe += inter2->noe; //updating number of members after the 2 team union 
							tmp = inter2->q;    //absorbed list
							while(tmp != NULL)
							{
								n = lookupNode(tmp->ID,g);
								ptmp = n->obj;
								index = binary_search(ptmp->interests[0],ptmp->inter_num,p1->interests[0][c2]);
								ptmp->interests[1][index] = inter1->tag;    //absorbing list
								if (tmp->next == NULL)  // unifying teams
								{
									tmp->next = inter1->q;
									inter1->q = inter2->q;
									break;
								}
								else
									tmp = tmp->next;
							}
							if (prev2 != NULL)  
							{
								prev1 = prev2->next;
								prev2->next = prev2->next->next;
							}
							else   
							{
								prev1 = obj->team;
								obj->team = obj->team->next;
							}
							free(prev1->iNode); //freeing the absorbed list
							free(prev1);
						}
						if(c1 < sum1-1) //checking for end of interest list
							c1++;
						c2++;
					}
					else if(p->interests[0][c1] < p1->interests[0][c2] && c1 < sum1-1) //checking for common interests
						c1++;
					else
					{
						if (p1->interests[1][c2] != 0)
						{
							c2++;
							continue;
						}
						inter = NULL;
						obj = NULL;
						if ((n = lookupNode(p1->interests[0][c2],interGraph)) == NULL) 
						{
							p1->interests[1][c2] = 1;   //marking
							inter = malloc(sizeof(Node));
							inter->ID = p1->interests[0][c2];
							inter->obj = malloc(sizeof(InterObj));
							obj = inter->obj;
							obj->mmax = 0;
							obj->wmax = 0;
							obj->next_num = 2; 
							obj->team = malloc(sizeof(InterList)); 
							obj->team->next = NULL;
							obj->team->iNode = malloc(sizeof(Interests)); 
							obj->team->iNode->tag = 1; 
							obj->team->iNode->noe = 1;
							obj->team->iNode->q = malloc(sizeof(Queue));
							obj->team->iNode->q->ID = n1->ID; 
							obj->team->iNode->q->next = NULL;
							insertNode(inter,interGraph);
						}
						else
						{
							obj = n->obj;
							p1->interests[1][c2] = obj->next_num;
							team = malloc(sizeof(InterList)); 
							team->next = obj->team;
							team->iNode = malloc(sizeof(Interests)); 
							team->iNode->tag = obj->next_num; 
							team->iNode->noe = 1; 
							team->iNode->q = malloc(sizeof(Queue)); 
							team->iNode->q->ID = n1->ID; 
							team->iNode->q->next = NULL;
							obj->next_num++;    
							obj->team = team;
						}
						c2++;
					}
				}
				if(p1->distN == -1) //checking if we visited the current node in the past
				{
					p1->distN = 1;
					end->next = malloc(sizeof(Queue)); 
					end->next->ID = n1->ID;
					end->next->next = NULL;
					end = end->next;
				}
			}
			l = l->next;
		}
		tmp = q;
		q = q->next;
		free(tmp);  
	}
	//find max and deleting lists
	for(i=0;i<interGraph->size;i++)
	{
		for(j=0;j<interGraph->position[i];j++)
		{
			obj = interGraph->table[i][j]->obj;
			if (obj->next_num == 1)
				continue;
			while(obj->team != NULL)
			{
				if (mode == 0)  //men's max
				{
					if(obj->mmax < obj->team->iNode->noe)
						obj->mmax = obj->team->iNode->noe;
				}
				else    //women's max
				{
					if(obj->wmax < obj->team->iNode->noe)
						obj->wmax = obj->team->iNode->noe;
				}
				while(obj->team->iNode->q != NULL)  
				{
					tmp = obj->team->iNode->q;
					obj->team->iNode->q = obj->team->iNode->q->next;
					free(tmp);
				}
				team = obj->team;
				free(obj->team->iNode); 
				obj->team = obj->team->next;
				free(team); 
			}
			obj->next_num = 1;  
			obj->team = NULL;
		}
	}
}

void matchTrends(int * array,int k,char *trends[])  //reading trends file and searching for trends
{
    FILE* fp = fopen("tag.csv", "r");
    if (fp == NULL)
    {
        perror ("Error opening file");
        exit(-1);
    }
    char buffer[200],*s;
    int ID,i;
    for(i=0;i<k;i++)    //k trends
    {
        fgets(buffer,200,fp);
        while(fgets(buffer,200,fp)!= NULL)
        {
            if ((ID = atoi(strtok (buffer,"|"))) == array[i]) 
            {
                s = strtok(NULL,"|");
                trends[i] = malloc(sizeof(char)*strlen(s)+1);
                strcpy(trends[i],s); 
                break;
            }
        }
        fseek (fp,0, SEEK_SET );
    }
    fclose(fp);
}

void findTrends(int k, Graph* g, char** womenTrends, char** menTrends)
{
	Graph * interGraph = createGraph(M,C);
	InterObj * obj;
	Person * p;
	int i,j,**array,**array2,m = 0,w = 0;
	array = malloc(2*sizeof(int*));     //men's trends
	array[0] = malloc(k*sizeof(int));
	array[1] = malloc(k*sizeof(int));
	array2 = malloc(2*sizeof(int*));    //women's trends
	array2[0] = malloc(k*sizeof(int));
	array2[1] = malloc(k*sizeof(int));
	for(i=0;i<g->size;i++)  //checking g
	{
		for(j=0;j<g->position[i];j++)
		{
			p = g->table[i][j]->obj;
			if (p->distN != -1) 
				continue;
			if (!strcmp("male",p->nProp->prop[2]))  
				groupInterests(g,interGraph,g->table[i][j],0);
			else
				groupInterests(g,interGraph,g->table[i][j],1);
		}
	}
	for(i=0;i<interGraph->size;i++) //for every interest
	{
		for(j=0;j<interGraph->position[i];j++)
		{
			obj = interGraph->table[i][j]->obj;
			if (m < k)  // checking number of interests
			{
				array[0][m] = interGraph->table[i][j]->ID;
				array[1][m] = obj->mmax;
				m++;
				if (m == k) //sorting
					array = insertionT(array,k);
			}
			else if (array[1][0] < obj->mmax)   //if number of interests == k, compare with the first element
			{
				array[0][0] = interGraph->table[i][j]->ID;
				array[1][0] = obj->mmax;
				array = insertionT(array,k);    //sorting
			}
			if (w < k)  //same for women's trends
			{
				array2[0][w] = interGraph->table[i][j]->ID;
				array2[1][w] = obj->wmax;
				w++;
				if (w == k)
					array2 = insertionT(array2,k);
			}
			else if (array2[1][0] < obj->wmax)
			{
				array2[0][0] = interGraph->table[i][j]->ID;
				array2[1][0] = obj->wmax;
				array2 = insertionT(array2,k);
			}
            //freeing resources
			free(obj);
			free(interGraph->table[i][j]);
		}
		free(interGraph->table[i]);
	}
    free(interGraph->table); 
    free(interGraph->cell_size);
    free(interGraph->position);
    free(interGraph); 
    matchTrends(array[0],k,menTrends);  
    matchTrends(array2[0],k,womenTrends);
    printf("\n\nMen Trends:\n");
    for(i=0;i<k;i++)
        printf("%s\n",menTrends[i]);
    printf("\nWomen Trends:\n");
    for(i=0;i<k;i++)
        printf("%s\n",womenTrends[i]);
    for(i=0; i < g->size; i++)
    {
    	for(j=0; j < g->position[i]; j++)
    	{
    		p = g->table[i][j]->obj;
    		p->distN = -1;
    	}
    }
    printf("\n");
    //freeing arrays
	free(array[0]);
	free(array[1]);
	free(array2[0]);
	free(array2[1]);
	free(array);
	free(array2);
}
