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
// Error code constants
#define ERR_INPUTFILE_CANNOTOPEN 100
#define ERR_INPUTFILE_CORRUPT_LINE 101
#define ERR_INPUTFILE_CORRUPT_LINES 102
#define ERR_INPUT_NVERTICES_INSUFF 201
#define ERR_PQ_UNDERFLOW 302

/*
   Data structure type defs for PriorityQueue (min-heap) and Graph
*/

typedef struct Vertex
{
    int VRTX;
    int COST;
    struct Vertex * NEXT;
} Vertex;

typedef struct
{
    Vertex ** List; // supposed to be an array of linked lists
    int * pred;
    int * dist;
    unsigned int n;
} Graph;

typedef struct
{
    int * heap;
    int * index;
    int * key;
    int sizePQ;
} PriorityQueue;

void AnnihilateList(Vertex * listhead);
int ExtractMin(PriorityQueue * PQ);
void PQUnderflow();
void Heapify(PriorityQueue * PQ, int r);
void DecreaseKey(PriorityQueue * PQ, int l, int newkey);
bool isEmptyPQ(PriorityQueue * PQ);
void InitPQ(Graph * G, PriorityQueue * PQ, int s);
void Dijkstra(Graph * G, PriorityQueue * PQ, int s);

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
                v->COST = tempInt;
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

    // Print G

    // Run dijkstra's algorithm and print G->List

    int s, p;
    for (s = 1; s <= G->n; s++)
    {
        Dijkstra(G, PQ, s);
        printf("\nDIJKSTRA RESULTS::Vertex %d:\n", s);
        printf("----------------------------\n");
        for (j = 1; j <= G->n; j++)
        {
            if (j != s)
            {
                printf("   Vertex %d to %d: cost = %d\n", s, j, dist[j]);
                printf("                  pred = %d\n\n", pred[j]);
            }
        }
    }
    

    // Clean up before exiting
    //printf("\n$ Begin Cleanup Process...\n");
    //printf("$ Iterating over list of pointers at %d...\n", list);
    for (i = 1; i <= G->n; i++)
    {
        AnnihilateList(list[i]);
    }
    //printf("$ Freeing graph at %d...\n", G);
    free(G);
    //printf("$ Freeing priority queue at %d...\n", PQ);
    free(PQ);
    return 0;
}

void AnnihilateList(Vertex * listhead)
{
    Vertex * temp1;
    while (listhead)
    {
        temp1 = listhead->NEXT;
        //printf("$ Freeing vertex (data: VRTX = %d; COST = %d;) at %d...\n", listhead->VRTX, listhead->COST, listhead);
        free(listhead);
        listhead = temp1;
    }
}

int ExtractMin(PriorityQueue * PQ)
{
    int j;
    if (PQ->sizePQ == 0)
    {
        PQUnderflow();
        exit(ERR_PQ_UNDERFLOW);
    }
    else
    {
        j = PQ->heap[1]; // retrieve return value (min-heap's root) before replacing root
        PQ->heap[1] = PQ->heap[PQ->sizePQ];
        PQ->index[PQ->heap[1]] = 1;
        PQ->sizePQ--;
        Heapify(PQ, 1);
    }

    /*int i;
    printf("PRIORITY QUEUE:\n------------------------\n");
    for (i = 1; i <= PQ->sizePQ; i++)
    {
        printf("heap: %d + key: %d, ", PQ->heap[i], PQ->key[PQ->heap[i]]);
    }
    printf("\n\n");*/

    return j;
}

bool isEmptyPQ(PriorityQueue * PQ)
{
    return (PQ->sizePQ == 0);
}

void PQUnderflow()
{
    fprintf(stderr, "ERR: Priority Queue Underflow: The min-heap is empty.\n");
}

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
            j += 1;
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

    /*int x;
    printf("PRIORITY QUEUE:\n------------------------\n");
    for (x = 1; x <= PQ->sizePQ; x++)
    {
        printf("heap: %d + key: %d, ", PQ->heap[x], PQ->key[PQ->heap[x]]);
    }
    printf("\n\n");*/

    PQ->heap[i] = l;
    PQ->index[l] = i;
}

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
    /*int x;
    printf("PRIORITY QUEUE:\n------------------------\n");
    for (x = 1; x <= PQ->sizePQ; x++)
    {
        printf("heap: %d + key: %d, ", PQ->heap[x], PQ->key[PQ->heap[x]]);
    }
    printf("\n\n");*/
}

void InitPQ(Graph * G, PriorityQueue * PQ, int s)
{
    int i = 1;
    int v;
    for (v = 1; v <= G->n; v++)
    {
        if (v == s)
        {
            PQ->heap[1] = s;
            PQ->index[s] = 1;
            PQ->key[s] = 0;
        }
        else
        {
            i++;
            PQ->heap[i] = v;
            PQ->index[v] = i;
            PQ->key[v] = 9999;
        }
    }
    PQ->sizePQ = G->n;
}

void Dijkstra(Graph * G, PriorityQueue * PQ, int s)
{
    int u, v, newval, i, j;
    Vertex * alpha;
    InitPQ(G, PQ, s); // Initialize priority queue (see definition of InitPQ)

    /*printf("$ Starting Dijkstra's algorithm process...\n");
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
    printf("\n     In G.dist: {");*/
    for (i = 1; i <= G->n; i++)
    {
        G->dist[i] = 0;
        G->pred[i] = 0;
        //printf("%d, ", G->dist[i]);
    }
    //printf("}\n\n");

    // Find shortest paths from vertex s to all other vertices
    while (isEmptyPQ(PQ) == false)
    {
        //printf("\n+++++++++++++++++++++++++++++++++++");
        u = ExtractMin(PQ);
        if (PQ->key[u] == 9999)
        {
            fprintf(stderr, "WRNG: No more edges to consider. Dijkstra exiting...");
            return;
        }
        //printf("\n%d",u);
        alpha = G->List[u];
        while (alpha != NULL)
        {
            //printf("\n----------------------------------");
            v = alpha->VRTX;
            newval = (PQ->key[u] >= 9999) ? alpha->COST : PQ->key[u] + alpha->COST;
            //printf("\n > %d %d %d", v, PQ->key[v], newval);
            if (PQ->key[v] > newval)
            {
                G->pred[v] = u;
                DecreaseKey(PQ, v, newval);
                //printf("\n @ %d %d %d", v, PQ->key[v], newval);
                /*printf("\n     In PQ.key: {");
                for (i = 1; i <= PQ->sizePQ; i++)
                {
                    printf("%d, ", PQ->key[i]);
                }
                printf("}\n");*/
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