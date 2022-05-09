#include <xinu.h>

//TODO - add in your functions
#include <stdio.h>
#include <stdlib.h>


//#define NLOCK 10
//#define NPROC 20
#define LOCK 0
#define PROC 1

struct AdjList* stack;


/**
 *
 *
 */
struct AdjListNode* newAdjListNode(int id, int lockOrProc)
{
    AdjListNode* newNode
        = (AdjListNode*)malloc(
            sizeof(AdjListNode));
    newNode->id = id;
    newNode->lockOrProc = lockOrProc;
    newNode->next = NULL;
    return newNode;
}

/**
 *
 *
 */
struct Graph* createGraph()
{
    struct Graph* graph
        = (struct Graph*)malloc(sizeof(struct Graph));
    graph->V = NLOCK + NPROC;

    stack = (struct AdjList *)malloc(sizeof(struct AdjList));
    stack->head = NULL;
 
    // Create an array of adjacency lists.  Size of
    // array will be V
    graph->array = (struct AdjList*)malloc((NLOCK + NPROC) * sizeof(struct AdjList));
 
    // Initialize each adjacency list as empty by
    // making head as NULL
    int i;
    for (i = 0; i < NPROC; i++){
        graph->array[i].id = i;
        graph->array[i].lockOrProc = PROC;
        graph->array[i].head = NULL;
    }
    for (i = NPROC; i < (NPROC + NLOCK); i++){
        graph->array[i].id = i - NPROC;
        graph->array[i].lockOrProc = LOCK;
        graph->array[i].head = NULL;
    }
 
    return graph;
}

void freeGraph(Graph *graph)
{
    AdjListNode *check = NULL;
    AdjListNode *prev = NULL;
    int i;
    for (i = 0; i < (NPROC + NLOCK); i++)
    {
        check = graph->array[i].head;
        while (check->next != NULL)
        {
            prev = check;
            check = check->next;
            free(prev, sizeof(struct AdjListNode));
        }
    }
    free(graph->array, (NPROC + NLOCK)*sizeof(struct AdjList));
    free(graph, sizeof(struct Graph));
}




/**
 * adds a request edge to the RAG from pid to lockid
 *
 */ 
void rag_request(Graph *graph, int pid, int lockid)
{
    // create a node
    AdjListNode *newNode = newAdjListNode(lockid, LOCK);
    // search for source in array (linear)
    int i;
    for (i = 0; i < NPROC; i++)
    {
        if (graph->array[i].id == pid){
            // figure out where in it's list to put it
            newNode->next = graph->array[i].head;
            graph->array[i].head = newNode;
        }
    }
    

}

/**
 *
 *
 */
void rag_alloc(Graph *graph, int pid, int lockid)
{
    // create a node
    AdjListNode *newNode = newAdjListNode(pid, PROC);
    // search for source in array (linear)
    int i;
    for (i = NPROC; i < (NPROC + NLOCK); i++)
    {
        //printf("%d\n",lockid);
        //printf("%d\n",graph->array[i].id);
        if (graph->array[i].id == lockid){
            //printf("Hellor\n");
            // figure out where in it's list to put it
            newNode->next = graph->array[i].head;
            graph->array[i].head = newNode;
        }
    }

    
    // remove request edge
    AdjListNode *check = NULL;
    AdjListNode *prev = NULL;
    for (i = 0; i < NPROC; i++)
    {
        if (graph->array[i].id == pid){
            // figure out where in it's list to put it
            check = graph->array[i].head;
            while (check != NULL)
            {
                if (check->id == lockid && check->lockOrProc == LOCK)
                {
                    //printf("we found it\n");
                    if (prev == NULL)
                    {
                        graph->array[i].head = check->next;
                    }
                    else{
                        prev->next = check->next;
                    }
                    free(check, sizeof(struct AdjListNode));
                    break;
                    
                }
                prev = check;
                check = check->next;
            }
        }
    }
    
}
/**
 *
 *
 */
void delete_edges(Graph *graph, int lockid){
    // remove request edge
    AdjListNode *check = NULL;
    AdjListNode *prev = NULL;
    int i;
    for (i = 0; i < NPROC; i++)
    {
        // figure out where in it's list to put it
        check = graph->array[i].head;
        while (check != NULL)
        {
            if (check->id == lockid && check->lockOrProc == LOCK)
            {
                //printf("we found it\n");
                if (prev == NULL)
                {
                    graph->array[i].head = check->next;
                }
                else{
                    prev->next = check->next;
                }
                free(check, sizeof(struct AdjListNode));
                break;
                
            }
            prev = check;
            check = check->next;
        }
    }

    int trueid = lockid + NPROC;
    check = graph->array[trueid].head;
    while (check != NULL)
    {
        graph->array[trueid].head = check->next;
        free(check, sizeof(struct AdjListNode));
        check = graph->array[trueid].head;
    }
}

void delete_proc_edges(Graph *graph, int pid)
{
    // remove allocation edge
    AdjListNode *check = NULL;
    AdjListNode *prev = NULL;
    int i;
    for (i = NPROC; i < (NPROC + NLOCK); i++)
    {
        // figure out where in it's list to put it
        check = graph->array[i].head;
        while (check != NULL)
        {
            if (check->id == pid && check->lockOrProc == PROC)
            {
                //printf("we found it\n");
                if (prev == NULL)
                {
                    graph->array[i].head = check->next;
                }
                else{
                    prev->next = check->next;
                }
                free(check, sizeof(struct AdjListNode));
                break;
                
            }
            prev = check;
            check = check->next;
        }
    }

    // remove all request edges
    check = graph->array[pid].head;
    while (check != NULL)
    {
        graph->array[pid].head = check->next;
        free(check, sizeof(struct AdjListNode));
        check = graph->array[pid].head;
    }
}

/**
 *
 *
 */
void rag_dealloc(Graph *graph, int pid, int lockid)
{
    // remove allocation edge
    AdjListNode *check = NULL;
    AdjListNode *prev = NULL;
    int i;
    for (i = NPROC; i < (NPROC + NLOCK); i++)
    {
        if (graph->array[i].id == lockid){
            // figure out where in it's list to put it
            check = graph->array[i].head;
            while (check != NULL)
            {
                if (check->id == pid && check->lockOrProc == PROC)
                {
                    if (prev == NULL)
                    {
                        graph->array[i].head = check->next;
                    }
                    else{
                        prev->next = check->next;
                    }
                    free(check, sizeof(struct AdjListNode));
                    break;
                    
                }
                prev = check;
                check = check->next;
            }
        }
    }
}

/**
 *
 *
 */
void rag_print(Graph *graph)
{
    printf("Printing...\n");
    int i;
    AdjListNode *check = NULL;
    for (i = 0; i < NPROC; i++)
    //for ( i = 0; i < 5; i++)
    {
        check = graph->array[i].head;
        printf("Node pid=%d -> { ", i);
        while (check != NULL)
        {
            printf("lockid=%d ", check->id);
            check = check->next;
        }
        printf("}\n");
    }

    for ( i = NPROC; i < (NPROC + NLOCK); i++)
    //for ( i = 5; i < 8; i++)
    {
        check = graph->array[i].head;
        printf("Node lockid=%d -> { ", graph->array[i].id);
        while (check != NULL)
        {
            printf("pid=%d ", check->id);
            check = check->next;
        }
        printf("}\n");
    }
}

/**
 *
 *
 */
int deadlock_detect(Graph *g)
{
    kprintf("Detecting\n");
    int visited[g->V];
    int rec[g->V];
    for (int i = 0; i < g->V; i++)
    {
        visited[i] = 0;
        rec[i] = 0;
    }

    int returnlock;
    for (int i = 0; i < g->V; i++)
    {
        if (isCycle(i, visited, rec, g))
        {
            int remember = stack->head->id;
            if (stack->head->lockOrProc == LOCK)
            {
                remember += NPROC;
            }
            AdjListNode *check = stack->head->next;
            printf("DEADLOCK\t");
            if (stack->head->lockOrProc == LOCK)
            {
                printf("lockid=%d ", stack->head->id);
            }
            else
            {
                printf("pid=%d ", stack->head->id);
            }
            while(check != NULL)
            {
                if (check->lockOrProc == LOCK)
                {
                    if (remember == (check->id + NPROC)){
                        printf("lockid=%d ", check->id);
                        break;
                    }
                    printf("lockid=%d ", check->id);
                }
                else{
                    if (remember == check->id){
                        printf("pid=%d ", check->id);
                        break;
                    }
                    printf("pid=%d ", check->id);
                }

                check = check->next;
            }
            printf("\n");
            
            check = stack->head;
            while (check != NULL)
            {
                stack->head = check->next;
                if (stack->head->lockOrProc == LOCK)
                {
                    returnlock = stack->head->id;
                }
                free(check, sizeof(struct AdjListNode));
                check = stack->head;
            }
            free(stack, sizeof(struct AdjList));
            deadlock_recover(returnlock);
            return 1;
        }
    }
    return 0;
}


int isCycle(int i, int *visited, int *recStack, Graph *graph)
{
    if (recStack[i])
    {
        return 1; // true
    }

    if (visited[i])
    {
        return 0; // false
    }

    visited[i] = 1;
    recStack[i] = 1;

    AdjList *array = graph->array;

    /*
    int j;
    for (j = 0; j < graph->V; j++)
    {
        if (array[i].head != NULL)
        {
            printf("%d ", array[i].head->id);
        }
    }
    printf("\n");
    */

    AdjListNode *check = array[i].head;

    
    // add node to stack
    //AdjListNode *stackNode = newAdjListNode(check->id, check->lockOrProc);
    //stackNode->next = stack->head;
    //stack->head = stackNode;
    

    while (check != NULL)
    {
        AdjListNode *stackNode = newAdjListNode(check->id, check->lockOrProc);
        stackNode->next = stack->head;
        stack->head = stackNode;
        int iCalc = check->id;
        if (check->lockOrProc == LOCK)
        {
            iCalc += NPROC;
        }
        if (isCycle(iCalc, visited, recStack, graph))
        {
            return 1;
        }

        check = check->next;
    }
    recStack[i] = 0;
    return 0;
}

void deadlock_recover(int lockid)
{
    //kprintf("Recovering lockid=%d\n", lockid);
    struct	lockentry *lptr;

    lptr = &locktab[lockid];

    // remove all processes from wait queue and add to ready queue
    pid32 temp;
	while (nonempty(lptr->wait_queue))
	{
		temp = dequeue(lptr->wait_queue);
        //kprintf("Dequeued pid=%d\n", temp);
		enqueue(temp, readyqueue, proctab[temp].prprio);
	}

    // find id of process that holds the lock
    pid32 pid = rag->array[lockid+NPROC].head->id;
    //kprintf("Killing pid=%d\n", pid);
    kill(pid);

    //kprintf("Removing process from wait queues\n");
    // remove process from lock queues
    int i;
    for (i = NPROC; i < (NPROC + NLOCK); i++)
    {
        remove(pid, lptr->wait_queue);
    }

    // unlock the lock
    mutex_unlock(&lptr->lock);

    // remove all edges
    delete_proc_edges(rag, pid);

    // print message
    kprintf("DEADLOCK RECOVER\tKilling pid=%d to release lockid=%d\n", pid, lockid);
}
