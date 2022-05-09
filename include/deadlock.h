//TODO - add your deadlock function prototypes

//TODO - add an "extern" declaration for the RAG
extern struct 	Graph	*rag;

typedef struct AdjListNode {
    int id;
    int lockOrProc;
    struct AdjListNode* next;
}AdjListNode;
 
// A structure to represent an adjacency list
typedef struct AdjList {
    int id;
    int lockOrProc;
    struct AdjListNode* head;
}AdjList;
 

typedef struct Graph {
    int V;
    struct AdjList* array;
}Graph;

struct Graph* createGraph();
void freeGraph(Graph *graph);
struct AdjListNode* newAdjListNode(int dest);
void rag_request(Graph *graph, int pid, int lockid);
void rag_alloc(Graph *graph, int pid, int lockid);
void rag_dealloc(Graph *graph, int pid, int lockid);
void rag_print(Graph *graph);
int deadlock_detect(Graph *graph);
int isCycle(int i, int *visited, int *recStack, Graph *graph);
