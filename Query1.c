#include "Query1.h"

void hpsort(int arr[], int N) //Heap sort algorithm
{
    int t; /* the temporary value */
    int n = N, parent = N/2, index, child; /* heap indexes */
    /* loop until array is sorted */
    while (1) {
        if (parent > 0) {
            /* first stage - Sorting the heap */
            t = arr[--parent];  /* save old value to t */
        } else {
            /* second stage - Extracting elements in-place */
            n--;                /* make the heap smaller */
            if (n == 0) {
                return; /* When the heap is empty, we are done */
            }
            t = arr[n];         /* save lost heap entry to temporary */
            arr[n] = arr[0];    /* save root entry beyond heap */
        }
        /* insert operation - pushing t down the heap to replace the parent */
        index = parent; /* start at the parent index */
        child = index * 2 + 1; /* get its left child index */
        while (child < n) {
            /* choose the largest child */
            if (child + 1 < n  &&  arr[child + 1] > arr[child]) {
                child++; /* right child exists and is bigger */
            }
            /* is the largest child larger than the entry? */
            if (arr[child] > t) {
                arr[index] = arr[child]; /* overwrite entry with child */
                index = child; /* move index to the child */
                child = index * 2 + 1; /* get the left child and go around again */
            } else {
                break; /* t's place is found */
            }
        }
        /* store the temporary value at its new location */
        arr[index] = t;
    }
}

void addInterests(char * file, Graph * g) //finding interests of every node
{
    FILE * fp = fopen(file,"r");
    if (fp == NULL)
    {
        perror ("Error opening file");
        exit(-1);
    }
    int tmpID,pID;
    char buffer[200];
    Node * n;
    InterQueue * tmp, * iqueue = NULL,*point,*previous;
    Queue * q;
    Person * person;
    fgets(buffer,200,fp);
    while(fgets(buffer,200,fp)!= NULL) 
    {
        pID = atoi(strtok (buffer,"|"));
        if((n = lookupNode(pID, g)) == NULL) //checking ID validity 
        {
            perror("Invalid person ID");
            exit(-1);
        }
        tmpID = atoi(strtok (NULL, "\n")); //tmpID = interest ID
        if(iqueue == NULL) 
        {
            iqueue = malloc(sizeof(InterQueue));
            iqueue->ID = pID;
            iqueue->q = malloc(sizeof(Queue)); //q = interests queue for current node
            iqueue->q->ID = tmpID; //q->ID = interest ID
            iqueue->q->next = NULL;
            iqueue->next = NULL;
            iqueue->counter = 1; //counter = interests total number for current node
        }
        else
        {
            point = iqueue;
            previous = NULL;
            while(point != NULL)
            {
                if(point->ID == pID)
                {
                    point->counter++; 
                    q = malloc(sizeof(Queue)); 
                    q->ID = tmpID;
                    q->next = point->q;
                    point->q = q;
                    break;
                }
                if(point->ID > pID) 
                {
                    tmp = malloc(sizeof(InterQueue));
                    tmp->ID = pID;
                    tmp->next = point;
                    tmp->counter = 1;
                    if (previous != NULL) //not first element of the queue
                        previous->next = tmp; //insert new person
                    else //start of the queue
                        iqueue = tmp;
                    tmp->q = malloc(sizeof(Queue));
                    tmp->q->ID = tmpID;
                    tmp->q->next = NULL;
                    break;
                }
                else if (point->next == NULL) //insert new person in the end of the queue
                {
                    point->next = malloc(sizeof(InterQueue));
                    point->next->ID = pID;
                    point->next->next = NULL;
                    point->next->counter = 1;
                    point->next->q = malloc(sizeof(Queue));
                    point->next->q->ID = tmpID;
                    point->next->q->next = NULL;
                    break;
                }
                else
                {
                    previous = point;
                    point = point->next;
                }
            }
        }
    }
    tmp = iqueue;
    int i = 0;
    while(tmp != NULL)
    {
        q = tmp->q;
        while(q!=NULL)
        {
            i++;
            q = q->next;
        }
        tmp = tmp->next;
    }
    while(iqueue != NULL)
    {
        //insert interests in graph G
        n = lookupNode(iqueue->ID,g);
        person = n->obj;
        person->inter_num = iqueue->counter;
        person->interests = malloc(2*sizeof(int *));
        person->interests[0] = malloc(person->inter_num*sizeof(int));
        person->interests[1] = malloc(person->inter_num*sizeof(int));
        for(i = 0;i<person->inter_num;i++)
        {
            person->interests[0][i] = iqueue->q->ID; // interest ID
            person->interests[1][i] = 0; //Query 3 field
            q = iqueue->q;
            iqueue->q = iqueue->q->next;
            free(q); 
        }
        hpsort(person->interests[0],person->inter_num); //sort interests 
        tmp = iqueue;
        iqueue = iqueue->next;
        free(tmp); 
    }
    fclose(fp);
}

//* QUERY 1 *//

void buildGraph(char * placesF, char * studiesF, char * worksF, Graph * secGraph) //infos graph creation (study,work,location)
{
    int tmpID;
    FILE * fp = fopen(placesF,"r");
    if (fp == NULL)
    {
        perror ("Error opening places file");
        exit(-1);
    }
    Node * n;
    SecInfo * info;
    char buffer[200],* s;
    fgets(buffer,200,fp);
    while(fgets(buffer,200,fp)!= NULL) //creation of graph and addition of location for every person
    {
        s = strtok (buffer,"|");
        n = malloc(sizeof(Node));
        n->ID = atoi(s);
        info = malloc(sizeof(SecInfo));
        info->place = atoi(strtok (NULL,"\n"));
        info->studies = NULL;
        info->works = NULL;
        n->obj = info;
        insertNode(n,secGraph);
    }
    fclose(fp);
    fp = fopen(studiesF,"r");
    if (fp == NULL)
    {
        perror ("Error opening studies file");
        exit(-1);
    }
    Queue * tmp, * point, *previous;
    fgets(buffer,200,fp);
    while(fgets(buffer,200,fp)!= NULL) //study infos
    {
        s = strtok (buffer,"|");
        if((n = lookupNode(atoi(s), secGraph)) == NULL)
        {
            perror("Invalid node ID");
            exit(-1);
        }
        info = n->obj;
        tmpID = atoi(strtok (NULL, "\n"));
        if(info->studies == NULL)
        {
            info->studies = malloc(sizeof(Queue));
            info->studies->ID = tmpID;
            info->studies->next = NULL;
        }
        else
        {
            point = info->studies;
            previous = NULL;
            while(point != NULL)
            {
                if(point->ID > tmpID)
                {
                    tmp = malloc(sizeof(Queue));
                    tmp->ID = tmpID;
                    tmp->next = point;
                    if (previous != NULL)
                        previous->next = tmp;
                    else
                        info->studies = tmp;
                    break;
                }
                else if (point->next == NULL)
                {
                    point->next = malloc(sizeof(Queue));
                    point->next->ID = tmpID;
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
    }
    fclose(fp);
    fp = fopen(worksF,"r");
    if (fp == NULL)
    {
        perror ("Error opening works file");
        exit(-1);
    }
    fgets(buffer,200,fp);
    while(fgets(buffer,200,fp)!= NULL) //work infos addition
    {
        s = strtok (buffer,"|");
        if((n = lookupNode(atoi(s), secGraph)) == NULL)
        {
            perror("Invalid node ID");
            exit(-1);
        }
        info = n->obj;
        tmpID = atoi(strtok (NULL, "|"));
        if(info->works == NULL)
        {
            info->works = malloc(sizeof(Queue));
            info->works->ID = tmpID;
            info->works->next = NULL;
        }
        else
        {
            point = info->works;
            previous = NULL;
            while(point != NULL)
            {
                if(point->ID > tmpID)
                {
                    tmp = malloc(sizeof(Queue));
                    tmp->ID = tmpID;
                    tmp->next = point;
                    if (previous != NULL)
                        previous->next = tmp;
                    else
                        info->works = tmp;
                    break;
                }
                else if (point->next == NULL)
                {
                    point->next = malloc(sizeof(Queue));
                    point->next->ID = tmpID;
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
    }
    fclose(fp);
}

int findCommonInterests(int * inter1,int * inter2,int sum1,int sum2)
{
    int counter = 0,c1 = 0,c2 = 0;
    while(c1<sum1 && c2<sum2)
    {
        if (inter1[c1] == inter2[c2])
        {
            c1++;
            c2++;
            counter++;
        }
        else if(inter1[c1] < inter2[c2])
            c1++;
        else
            c2++;
    }
    return counter;
}

int traverseLSW(Queue * tmp,Queue * pivot)
{
    while (tmp != NULL && pivot != NULL) //common element not found
    {
        if (tmp->ID == pivot->ID) //if common element found, return 1
            return 1;
        else if (pivot->ID < tmp->ID) //if element of first list > element of second list, move the second pointer
            pivot = pivot->next;
        else //else, move the first pointer
            tmp = tmp->next;
    }
    return 0;
}

int checkLSW(SecInfo * s1,SecInfo * s2) //check if they live in the same area, work together or study at the same organisation
{
    Queue * tmp = s1->studies, * pivot = s2->studies;
    if (traverseLSW(tmp,pivot)) //checking for common study organisation
        return 3;
    tmp = s1->works;
    pivot = s2->works;
    if (traverseLSW(tmp,pivot)) //work
        return 2;
    return s1->place == s2->place; //living place
}

Matches * insertionS(Matches * arr,int num) //Insertion sort
{
    int i,tmp;
    Matches swap;
    for (i = 1 ; i < num; i++) {
        tmp = i;
        while (tmp > 0 && arr[tmp].score < arr[tmp-1].score) {
            swap = arr[tmp];
            arr[tmp] = arr[tmp-1];
            arr[tmp-1] = swap;
            tmp--;
        }
    }
    return arr;
}

Matches* findMatches(Node* n, int k, int h, int x, int limit, Graph* g,Graph* secg) //find possible matches for node n based on common interests
{
    Person * p,* p1,* p2;
    int com_inter,entry = 0,i,sum1,sum2;
    double temp;
    p = n->obj;
    if (p->inter_num < k) //number of interests of node n < k -> 0 matches
        return NULL;
    Matches * matches = malloc(limit * sizeof(Matches));
    Queue *start = NULL,*end,*list = NULL;
    int * inter1,*inter2;
    Node * n1,*n2;
    List * neighbor;
    SecInfo * s,*s1;
    if (p->edges == 0) //if n has not neighbors
        return NULL;
    p->pair->ID = 1;
    neighbor = p->list;
    list = malloc(sizeof(Queue)); //possible matches list
    list->ID = neighbor->neighbor->ID;
    list->next = NULL;
    n1 = lookupNode(list->ID, g);
    p1 = n1->obj;
    p1->pair->distance = 1;
    p1->pair->ID = 1;
    neighbor = neighbor->next;
    start = list;
    for(i = 1;i< p->edges;i++)
    {
        list->next = malloc(sizeof(Queue)); //insertion of neighbors
        list->next->ID = neighbor->neighbor->ID;
        list->next->next = NULL;
        n1 = lookupNode(list->next->ID, g);
        p1 = n1->obj;
        p1->pair->distance = 1; //search depth
        p1->pair->ID = 1; //marking node
        list = list->next;
        neighbor = neighbor->next;
    }
    end = list;
    inter1 = p->interests[0]; //interests array
    sum1 = p->inter_num; //number of interests
    n = lookupNode(n->ID,secg);
    s = n->obj;
    list = start;
    while(list != NULL)
    {
        n1 = lookupNode(list->ID,g); 
        p1 = n1->obj;
        inter2 = p1->interests[0]; 
        sum2 = p1->inter_num;
        if (p1->inter_num < k) 
        {
            list = list->next;
            continue;
        }
        if (p1->pair->distance > h) //if search depth > given depth
            break;
        neighbor = p1->list;
        for(i = 0;i< p1->edges;i++) 
        {
            n2 = lookupNode(neighbor->neighbor->ID, g);
            p2 = n2->obj;
            if (p2->pair->ID != -1)
            {
                neighbor = neighbor->next;
                continue;
            }
            end->next = malloc(sizeof(Queue));
            end->next->ID = neighbor->neighbor->ID;
            end->next->next = NULL;
            p2->pair->distance = p1->pair->distance+1;
            p2->pair->ID = 1;
            end = end->next;
            neighbor = neighbor->next;
        }
        if ((com_inter=findCommonInterests(inter1,inter2,sum1,sum2)) < k) 
        {
            list = list->next;
            continue;
        }
        n1 = lookupNode(list->ID,secg);
        s1 = n1->obj;
        if (checkLSW(s,s1) == 0) 
        {
            list = list->next;
            continue;
        }
        if (p->nProp->prop[3] >= p1->nProp->prop[3] && (p->nProp->prop[3] - p1->nProp->prop[3]) > x) 
        {
            list = list->next;
            continue;
        }
        else if (p1->nProp->prop[3] >= p->nProp->prop[3] && (p1->nProp->prop[3] - p->nProp->prop[3]) > x)
        {
            list = list->next;
            continue;
        }
        if (strcmp(p->nProp->prop[2],p1->nProp->prop[2])) // checking if n and n1 have different gender
        {  
            if (entry < limit) //we want only "limit" matches
            {
                matches[entry].ID = n1->ID;
                matches[entry].score = (double)com_inter/(p->inter_num + p1->inter_num - com_inter);
                entry++;
            }
            else //checking "score" of the node
            {
                temp = (double)com_inter/(p->inter_num + p1->inter_num - com_inter);
                if (matches[0].score >= temp)
                {
                    list = list->next;
                    continue;
                }
                matches[0].ID = n1->ID;
                matches[0].score = temp;
            }
            insertionS(matches,entry); //sorting matches
        }
        list = list->next;
    }
    freeList(start,n->ID,g);
    return matches;
}

int freeSWGraph(Graph *g) //freeing resources of study-work graph
{
    int i,j;
    Queue * tmp;
    SecInfo * obj;
    for (i=0;i<g->size;i++)
    {
        for(j=0;j<g->position[i];j++)
        {
            obj = g->table[i][j]->obj;
            while(obj->studies!= NULL)
            {
                tmp = obj->studies;
                obj->studies = obj->studies->next;
                free(tmp);
            }
            while(obj->works!= NULL) 
            {
                tmp = obj->works;
                obj->works = obj->works->next;
                free(tmp);
            }
            free(obj);
            free(g->table[i][j]); 
        }
        free(g->table[i]); 
    }
    free(g->table); 
    free(g->cell_size);
    free(g->position);
    free(g); 
    return 1;
}

Matches * matchSuggestion(Node* n, int k, int h, int x, int limit, Graph* g)
{
    Graph * swGraph = createGraph(M,C);
    Matches * matches;
    buildGraph("person_isLocatedIn_place.csv", "person_studyAt_organisation.csv", "person_workAt_organisation.csv", swGraph);
    matches = findMatches(n, k, h, x, limit, g, swGraph);
    freeSWGraph(swGraph);
    return matches;
}
