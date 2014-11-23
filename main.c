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
#include "headers.h"

#define STRINGMAX 100
#define INPUTFILENAME "graphmatrix.txt"
#define INFINITY 9999
// Error code constants
#define ERR_INPUTFILE_CANNOTOPEN 100
#define ERR_INPUTFILE_CORRUPT_LINE 101
#define ERR_INPUTFILE_CORRUPT_LINES 102
#define ERR_INPUT_NVERTICES_INSUFF 201

/*
   The heap implementation of a Priority Queue
   here is an just a single array of Vertices, each
   with an ID and a Cost (referred to as key in E.P. Quiwa)
*/

typedef struct
{
    int ID; // identifies the vertex
    unsigned int Cost;
} Vertex;

typedef struct
{
    unsigned int N; // number of vertices
    unsigned int * Cost;
    unsigned int * Pred;
    unsigned int * Dist;
} Graph;

void initPQ(Graph * G, Vertex * PQ, int s);

int main()
{
    printf("\n!++++++++++++++++++++++++++++++++++++++++++++++++++++!");
    printf("\n!                CS 32 MACHINE PROBLEM 3             !");
    printf("\n!                 DIJKSTRA'S ALGORITHM               !");
    printf("\n! Vincent Fiestada  2013-69155  vffiestada@up.edu.ph !");
    printf("\n!++++++++++++++++++++++++++++++++++++++++++++++++++++!\n");

    int numberOfVertices = 0;
	int i,j; // For loop iterators
	char temp[STRINGMAX] = ""; // For file reading
	FILE * inputFile = fopen(INPUTFILENAME, "r"); // open a stream to the file

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

    // Declare variables that are dependent on the numberOfVertices being intialized

	unsigned int Cost[numberOfVertices][numberOfVertices]; // Cost Adjacency matrix

	// Read the rest of the file
	for (i = 0; i < numberOfVertices; i++) // Read {numberOfVertices} Rows
	{
		for (j = 0; j < numberOfVertices; j++) // Read {numberOfVertices} Columns
		{
			fscanf(inputFile, "%d", &Cost[i][j]);
			if (feof(inputFile)) // Check if end-of-file indicator is set to true
			{
				fprintf(stderr, "\nFATAL ERR: Reached EOF unexpectedly.\n");
				exit(ERR_INPUTFILE_CORRUPT_LINES);
			}
		}
	}

	fclose(inputFile); // IMPORTANT: close the file stream

	// Print input
    printf("\nReading input from file '%s'...", INPUTFILENAME);
    printf("\nCOST ADJACENCY MATRIX OF %d VERTICES:\n", numberOfVertices);
	for (i = 0; i < numberOfVertices; i++)
	{
		for (j = 0; j < numberOfVertices; j++)
		{
			printf("%d ", Cost[i][j]);
		}
		printf("\n");
	}

    Vertex * PQ[numberOfVertices + 1]; // Priority Queue or min-heap
    unsigned int Pred[numberOfVertices];
    unsigned int Dist[numberOfVertices];
    Graph * G = malloc(sizeof(Graph)); // data about the Graph itself
    G->N = numberOfVertices;
    G->Cost = (unsigned int *)Cost;
    G->Pred = Pred;
    G->Dist = Dist;

    for (i = 0; i < numberOfVertices + 1; i++) // Populate the pool of vertices for PQ
    {
        PQ[i] = malloc(sizeof(Vertex));
    }

    // CLEAN UP; Free up the memory allocated for vertices and the struct G
    for (i = 0; i < numberOfVertices; i++)
    {
        free(PQ[i]);
    }
    free(G);
    printf("\n");
    return 0;
}

void initPQ(Graph * G, Vertex * PQ, int s)
{
    unsigned int i = 0;
    unsigned int v;
    for (v = 0; v < G->N; v++)
    {
        if (v == s)
        {
            PQ->ID = s; // PQ.heap(1) <- s;
            PQ->Cost = 0; // PQ.key(s) <- 0;
        }
        else
        {
            i++; // i <- i + 1
            Vertex * alpha = PQ + i*(sizeof(Vertex *));
            alpha->ID = v; // PQ.heap(i) <- v;
            alpha->Cost = INFINITY; // PQ.key(v) = Infinity;
        }
        PQ[G->N]->ID = -1; // The negative ID serves as a sort of terminating element
        // It is used to find out PQ's size or if it is empty
    }
}

Vertex * ExtractMin(Vertex * PQ)
{
    Vertex * j;
    Vertex * alpha;
    if (PQ->ID == -1)
    {
        // PQ is empty if first element is the terminator
        fprintf(stderr, "\nPriority Queue Underflow");
    }
    else
    {
        j = PQ; // Get root of heap
        // Now find the last element before the 
    }
}
