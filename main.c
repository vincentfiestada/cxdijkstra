/******************************************************************************
CS 32 Machine Problem # 3 - Dijkstra's Algorithm Iterated Implementation
                          - Based on "Data Structures" by Evangel P. Quiwa
                          - Finds the shortest path from a source to all other
                            vertices in a graph

Programmer: Vincent Paul F. Fiestada
Student Number: 2013-69155
BS Computer Science, University of the Philippines Diliman
Contact: vffiestada@up.edu.ph
******************************************************************************/
#include "cardinal.h"

#define STRINGMAX 100
#define INPUTFILENAME "graphmatrix.txt"
#define INFINITY 9999
// Error code constants
#define ERR_INPUTFILE_CANNOTOPEN 100
#define ERR_INPUTFILE_CORRUPT_LINE 101
#define ERR_INPUTFILE_CORRUPT_LINES 102
#define ERR_INPUT_NVERTICES_INSUFF 201

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
} PriorityQueue;

void AnnihilateList(Vertex * listhead);

int main()
{
    printf("\n!++++++++++++++++++++++++++++++++++++++++++++++++++++!");
    printf("\n!                CS 32 MACHINE PROBLEM 3             !");
    printf("\n!                 DIJKSTRA'S ALGORITHM               !");
    printf("\n! Vincent Fiestada  2013-69155  vffiestada@up.edu.ph !");
    printf("\n!++++++++++++++++++++++++++++++++++++++++++++++++++++!\n");

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

    // Initialize graph
	Graph * G = malloc(sizeof(Graph));
    Vertex * list[numberOfVertices];
    for (i = 0; i < numberOfVertices; i++)
    {
        list[i] = NULL;
    }
    int pred[numberOfVertices];
    int dist[numberOfVertices];
    G->List = list;
    G->pred = pred;
    G->dist = dist;
    G->n = numberOfVertices;

    // Declare Priority Queue (min-heap); stores class 2
    PriorityQueue * PQ = malloc(sizeof(PriorityQueue));

	// Read the rest of the file
	for (i = 0; i < numberOfVertices; i++) // Read {numberOfVertices} Rows
	{
		for (j = 0; j < numberOfVertices; j++) // Read {numberOfVertices} Columns
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

    // Print G->List

    Vertex * temp;
    for (i = 0; i < G->n; i++)
    {
        printf("Vertex %d:\n", i);
        temp = list[i];
        while (temp)
        {
            printf("\tCost to %d: %d\n", temp->VRTX, temp->COST);
            temp = temp->NEXT;
        }
    }

    // Clean up before exiting
    printf("\nBegin Cleanup Process...\n");
    printf("Iterating over list of pointers at %d...\n", list);
    for (i = 0; i < G->n; i++)
    {
        AnnihilateList(list[i]);
    }
    printf("Freeing graph at %d...\n", G);
    free(G);
    printf("Freeing priority queue at %d...\n", PQ);
    free(PQ);
    return 0;
}

void AnnihilateList(Vertex * listhead)
{
    Vertex * temp1;
    while (listhead)
    {
        temp1 = listhead->NEXT;
        printf("Freeing vertex (data: VRTX = %d; COST = %d;) at %d...\n", listhead->VRTX, listhead->COST, listhead);
        free(listhead);
        listhead = temp1;
    }
}