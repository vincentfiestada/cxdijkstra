/******************************************************************************
CS 32 Machine Problem # 3 - Dijkstra's Algorithm Iterated Implementation
                          - Based on "Data Structures" by Evangel P. Quiwa
                          - Finds the shortest path from a source to all other
                            vertices in a graph

Programmer: Vincent Paul F. Fiestada
Student Number: 2013-69155
BS Computer Science, University of the Philippines Diliman
Contact: vffiestada@up.edu.ph

Git Repo: https://github.com/vincentfiestada/cxdijkstra.git
******************************************************************************/
#include "cardinal.h"

#define STRINGMAX 100
#define INPUTFILENAME "graphmatrix.txt"
#define INFINITE 9999
// Error code constants
#define ERR_INPUTFILE_CANNOTOPEN 100
#define ERR_INPUTFILE_CORRUPT_LINE 101
#define ERR_INPUTFILE_CORRUPT_LINES 102
#define ERR_INPUT_NVERTICES_INSUFF 201
#define ERR_PQ_UNDERFLOW 301
// Define DEBUG when debugging to enable preliminary outputs
// #define DEBUG

/*
   Data structure type defs for PriorityQueue (min-heap) and Graph
*/

typedef struct Vertex // G.List(1:n) is an array of linked lists of Vertex structs
{
    int VRTX; // "name" of the Vertex (1, 2, 3, ..., n)
    int COST; // cost or weight of the path from the src to the Vertex
    struct Vertex * NEXT;
} Vertex;

typedef struct
{
    Vertex ** List; // supposed to be an array of linked lists
    int * pred; // Dijkstra's output is stored in pred and dist
    int * dist;
    unsigned int n;
} Graph;

typedef struct // More pointers representing arrays
{
    int * heap;
    int * index;
    int * key;
    unsigned int sizePQ;
} PriorityQueue;

typedef struct // simple stack implementation (sequential)
{
    int * Vector; // pointer to int array where stack data is stored
    unsigned int Length; // number of currently occupied slots
    unsigned int Capacity; // Max length
} Stack;

// Various function declarations
void AnnihilateList(Vertex * listhead);
int ExtractMin(PriorityQueue * PQ);
void PQUnderflow();
void Heapify(PriorityQueue * PQ, int r);
void DecreaseKey(PriorityQueue * PQ, int l, int newkey);
bool isEmptyPQ(PriorityQueue * PQ);
void InitPQ(Graph * G, PriorityQueue * PQ, int s);
void StackOverflow();
void StackUnderflow();
void Dijkstra(Graph * G, PriorityQueue * PQ, int s);
void PushToStack(Stack * S, int data);
int PopFromStack(Stack * S);

int main()
{
    printf("\n++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    printf("\n|                CS 32 MACHINE PROBLEM 3             |");
    printf("\n|                 DIJKSTRA'S ALGORITHM               |");
    printf("\n| Vincent Fiestada  2013-69155  vffiestada@up.edu.ph |");
    printf("\n++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

    int numberOfVertices = 0;
	int i, j; // For loop iterators
    int tempInt;
	FILE * inputFile = fopen(INPUTFILENAME, "r"); // open a stream to the file
    int EOF_Err_Counter = 0;

	if (!inputFile) // Handle errors in opening the file stream
	{
		fprintf(stderr, "\nFATAL ERR: Could not open the file 'graphmatrix.txt'.\n");
		exit(ERR_INPUTFILE_CANNOTOPEN);
	}
	// Read the first line
	fscanf(inputFile, "%d", &numberOfVertices);
	if (numberOfVertices < 1)
	{
		fprintf(stderr, "\nFATAL ERR: Number of Vertices is less than 1.");
		exit(ERR_INPUT_NVERTICES_INSUFF);
	}

    // declare and initialize graph
	Graph * G = malloc(sizeof(Graph));
    Vertex * list[numberOfVertices + 1];
    int pred[numberOfVertices + 1];
    int dist[numberOfVertices + 1];
    for (i = 1; i <= numberOfVertices; i++)
    {
        list[i] = NULL;
    }
    G->List = list;
    G->pred = pred;
    G->dist = dist;
    G->n = numberOfVertices;

    // Declare and initialize Priority Queue (min-heap); stores class 2
    PriorityQueue * PQ = malloc(sizeof(PriorityQueue));
    int heap[numberOfVertices + 1];
    int index[numberOfVertices + 1];
    int key[numberOfVertices + 1];
    PQ->heap = heap;
    PQ->index = index;
    PQ->key = key;
    PQ->sizePQ = numberOfVertices;

	// Read the rest of the file
	for (i = 1; i <= numberOfVertices; i++) // Read {numberOfVertices} Rows
	{
		for (j = 1; j <= numberOfVertices; j++) // Read {numberOfVertices} Columns
		{
			fscanf(inputFile, "%d", &tempInt);
            if (j != i)
            {
                Vertex * v = malloc(sizeof(Vertex));
                v->VRTX = j;
                v->COST = (tempInt == 0) ? INFINITE : tempInt;
                v->NEXT = NULL;

                // If list is empty, make head
                if (list[i] == NULL)
                {
                    list[i] = v;
                }
                else // else, traverse until tail is found
                {
                    Vertex * temp = list[i];
                    while (temp->NEXT)
                    {
                        temp = temp->NEXT;
                    }
                    temp->NEXT = v;
                }
            }
			if (feof(inputFile)) // Check if end-of-file indicator is set to true
			{
                EOF_Err_Counter++; // During testing, it seems that EOF may be triggered
                                   //  prematurely based on the formatting of the text file
                if (EOF_Err_Counter > 1)
                {
                    // If End Of File has been reached twice, then raise an error
				    fprintf(stderr, "\nFATAL ERR: Reached EOF unexpectedly.\n");
				    exit(ERR_INPUTFILE_CORRUPT_LINES);
                }
			}
		}
	}

	fclose(inputFile); // IMPORTANT: close the file stream

    int s, p; // s will keep track of the source, p is for the parsing predecessor Traceback
    Stack predecessorTraceback; // For tracing back the path to a vector
    int pT_vector[G->n]; // maximum of G->n ancestors will be stored
    predecessorTraceback.Vector = pT_vector;
    predecessorTraceback.Capacity = G->n;

    for (s = 1; s <= G->n; s++) // Do Dijkstra for each vertex
    {
        Dijkstra(G, PQ, s);
        printf("\nDIJKSTRA RESULTS >> Vertex %d as Source:\n", s);
        printf("----------------------------\n");
        for (j = 1; j <= G->n; j++)
        {
            predecessorTraceback.Length = 0; // reset stack
            if (j != s)
            {
                printf("   Vertex %d to %d: ", s, j);
                switch(dist[j]) // print cost if path exists
                {
                    case 0: // If dist is 0, then path does not exist
                        printf("(NO PATH)\n", dist[j]);
                    break;
                    default:
                        // add j to the stack of predecessors so it will be the final step in the 'path'
                        PushToStack(&predecessorTraceback, j);
                        p = pred[j]; // Trace back the path until p has no predecessor
                        while (p)
                        {
                            PushToStack(&predecessorTraceback, p);
                            p = pred[p];
                        }
                        // Now print the stack
                        while (predecessorTraceback.Length > 0)
                        {
                            printf("%d", PopFromStack(&predecessorTraceback));
                            if (predecessorTraceback.Length > 0)
                            {
                                printf(" -> ");
                            }
                        }
                        printf(" (cost = %d)\n", dist[j]);
                }
            }
        }
    }

    // Clean up before exiting
    #ifdef DEBUG
        printf("\n$ Begin Cleanup Process...\n");
        printf("$ Iterating over list of pointers at %d...\n", list);
    #endif
    for (i = 1; i <= G->n; i++)
    {
        AnnihilateList(list[i]);
    }
    #ifdef DEBUG
        printf("$ Freeing graph at %d...\n", G);
        printf("$ Freeing priority queue at %d...\n", PQ);
    #endif
    free(G);
    free(PQ);
    return 0;
}

// <summary>
//     Frees up all vertices in a singly-linked list
// </summary>
void AnnihilateList(Vertex * listhead)
{
    Vertex * temp1;
    while (listhead)
    {
        temp1 = listhead->NEXT;
        #ifdef DEBUG
            printf("$ Freeing vertex (data: VRTX = %d; COST = %d;) at %d...\n", listhead->VRTX, listhead->COST, listhead);
        #endif
        free(listhead);
        listhead = temp1;
    }
}

// <summary>
//     Extracts the minimum vertex from the root of the min-heap
//      and then re-heapifies the min-heap
// </summary>
int ExtractMin(PriorityQueue * PQ)
{
    int j;
    // check if there is something to extract first.
    if (PQ->sizePQ == 0) // isEmptyPQ can be used but Quiwa didn't so...meh.
    {
        PQUnderflow();
        exit(ERR_PQ_UNDERFLOW);
    }
    else
    {
        j = PQ->heap[1]; // retrieve return value (min-heap's root) before replacing root
        // put last element into root
        PQ->heap[1] = PQ->heap[PQ->sizePQ];
        PQ->index[PQ->heap[1]] = 1;
        PQ->sizePQ--;
        Heapify(PQ, 1); // run Heapify so the heap becomes a min-heap again
    }
    return j; // return min
}

// <summary>
//    returns true if the priority queue is empty; false otherwise
// </summary>
bool isEmptyPQ(PriorityQueue * PQ)
{
    return (PQ->sizePQ == 0);
}

// A bunch of Overflow and Underflow functions
void PQUnderflow()
{
    fprintf(stderr, "\nERR: Priority Queue Underflow: The min-heap is empty.\n");
}

void StackOverflow()
{
    fprintf(stderr, "\nERR: Stack Overflow: The stack is full. Push operation cancelled.\n");
}

void StackUnderflow()
{
    fprintf(stderr, "\nERR: Stack Underflow: The stack is empty. Pop operation cancelled.\n");
}

// <summary>
//      Turns a binary tree into a min-heap starting from input r and going
//          through the heap top-to-bottom
// </summary>
void Heapify(PriorityQueue * PQ, int r)
{
    int k = PQ->key[PQ->heap[r]];
    int l = PQ->heap[r];
    int i = r;
    int j = 2*i;
    while (j <= PQ->sizePQ)
    {
        if ((j < PQ->sizePQ) && (PQ->key[PQ->heap[j + 1]] < PQ->key[PQ->heap[j]]))
        {
            j++;
        }
        if (PQ->key[PQ->heap[j]] < k)
        {
            PQ->heap[i] = PQ->heap[j];
            PQ->index[PQ->heap[j]] = i;
            i = j;
            j = 2*i;
        }
        else
        {
            break;
        }
    }
    PQ->heap[i] = l;
    PQ->index[l] = i;
}

// <summary>
//     adds an int element to the top of a sequentially implemented stack
// </summary>
void PushToStack(Stack * S, int data)
{
    if (S->Length == S->Capacity) // If length is same as capacity, stack is full
    {
        StackOverflow();
    }
    else
    {
        S->Vector[S->Length++] = data; // add data to the end and increase length
    }
}

// <summary>
//     Pops from the top of a sequentially implemented stack
//        and returns the popped integer
// </summary>
int PopFromStack(Stack * S)
{
    if (S->Length == 0) // Check if stack is empty; cancel if yes
    {
        StackUnderflow();
        return -1;
    }
    else
    {
        return S->Vector[--S->Length]; // get data from the end/"top" and decrease length
    }
}

// <summary>
//    Decreases the key of an element in the priority queue/min-heap
//       and resorts the queue
//    Params: l = vertex whose key is to be changed; newkey = new key value
//            PQ = priority queue
// </summary>
void DecreaseKey(PriorityQueue * PQ, int l, int newkey)
{
    PQ->key[l] = newkey;
    int i = PQ->index[l];
    int j = floor(i/2);
    while ((i > 1) && (PQ->key[PQ->heap[j]] > newkey))
    {
        PQ->heap[i] = PQ->heap[j];
        PQ->index[PQ->heap[j]] = i;
        i = j;
        j = floor(i/2);
    }
    PQ->heap[i] = l;
    PQ->index[l] = i;
}

// <summary>
//    Initializes the Priority Queue
//    Sets the first element as the source and makes its key 0
//    Initializes all other elements/vertices with an INFINITE key
// </summary>
void InitPQ(Graph * G, PriorityQueue * PQ, int s)
{
    int i = 1;
    int v;
    for (v = 1; v <= G->n; v++) // Go through all vertices
    {
        if (v == s) // Set source as the root of the queue, with cost = 0
        {
            PQ->heap[1] = s;
            PQ->index[s] = 1;
            PQ->key[s] = 0;
        }
        else // all other vertices start with INFINITE cost
        {
            i++;
            PQ->heap[i] = v;
            PQ->index[v] = i;
            PQ->key[v] = INFINITE;
        }
    }
    PQ->sizePQ = G->n;
}

// <summary>
//     Dijkstra - the algorithm as described by Edsger Dijkstra in 1956
//                and as implemented by Evangel P. Quiwa in "Data Structures"
//              - finds the shortest path from vertex s to all other vertices in G
//              - PQ is also a necessary parameter; it is a priority queue that will be used
//                by the algorithm
//              - results are stored in G.dist and G.pred
// </summary>
void Dijkstra(Graph * G, PriorityQueue * PQ, int s)
{
    int u, v, newval, i, j;
    Vertex * alpha;
    InitPQ(G, PQ, s); // Initialize priority queue (see definition of InitPQ)

    #ifdef DEBUG
        printf("$ Starting Dijkstra's algorithm process...\n");
        printf("$ INPUT:\n--------------------------\n");
        for (i = 1; i <= G->n; i++)
        {
            alpha = G->List[i];
            printf("     In G.List(%d):\n          ", i);
            while(alpha)
            {
                printf("[VRTX: %d, COST: %d] > ", alpha->VRTX, alpha->COST);
                alpha = alpha->NEXT;
            }
            printf("\n");
        }
        printf("\n     In G.dist: {");
    #endif

    // Initialize G.dist and G.pred to arrays of zeroes
    for (i = 1; i <= G->n; i++)
    {
        G->dist[i] = 0;
        G->pred[i] = 0;
        #ifdef DEBUG
            printf("%d, ", G->dist[i]);
        #endif
    }
    #ifdef DEBUG
        printf("}\n\n");
    #endif

    // Find shortest paths from vertex s to all other vertices
    while (isEmptyPQ(PQ) == false)
    {
        u = ExtractMin(PQ);
        #ifdef DEBUG
            printf("\n+++++++++++++++++++++++++++++++++++");
            printf("\nExtracted Minimum from heap is %d.\n",u);
        #endif
        if (PQ->key[u] == INFINITE)
        {
            fprintf(stderr, "WRNG: No more edges to consider. Dijkstra returning...");
            return;
        }
        alpha = G->List[u];
        while (alpha != NULL) // Go through all vertices to update key values
        {
            v = alpha->VRTX;
            newval = (PQ->key[u] >= INFINITE) ? alpha->COST : PQ->key[u] + alpha->COST;
            #ifdef DEBUG
                printf("\n----------------------------------");
                printf("\n > %d %d %d", v, PQ->key[v], newval);
            #endif
            if (PQ->key[v] > newval)
            {
                G->pred[v] = u;
                DecreaseKey(PQ, v, newval);
                #ifdef DEBUG
                    printf("\n @ %d %d %d", v, PQ->key[v], newval);
                    printf("\n     In PQ.key: {");
                    for (i = 1; i <= PQ->sizePQ; i++)
                    {
                        printf("%d, ", PQ->key[i]);
                    }
                    printf("}\n");
                #endif
            }
            alpha = alpha->NEXT;
        }
    }
    // G.dist = PQ.key
    for (i = 1; i <= G->n; i++)
    {
        G->dist[i] = PQ->key[i];
    }
    return;
}