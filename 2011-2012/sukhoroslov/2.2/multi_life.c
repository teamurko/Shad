#include <math.h>
#include <assert.h>
#include <stdio.h>

#include "mpi.h"

const int NMAX = 105;

int main(int argc, char* argv[])
{
    freopen("input.txt", "rt", stdin);

    static const int NUM_NEIGHBOURS = 8;
    static const int dx[] = {0, -1, -1, -1, 0, 1, 1, 1};
    static const int dy[] = {1, 1, 0, -1, -1, -1, 0, 1};

    MPI_Status status;
    int numProcs, rank;
    int numRows, numColumns, numIterations;
    int startIndex, endIndex;
    double time;
    unsigned char field[NMAX][NMAX];

    int rc = MPI_Init(&argc, &argv);
    if (rc != MPI_SUCCESS) {
        printf("Error starting MPI program. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    if (numProcs <= 1) {
        printf("There should be at least 2 tasks.\n");
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        int rowIndex, columnIndex;
        time = -MPI_Wtime();
        scanf("%d %d %d\n", &numRows, &numColumns, &numIterations);
        for (rowIndex = 0; rowIndex < numRows; ++rowIndex) {
            for (columnIndex = 0; columnIndex < numColumns; ++columnIndex) {
                scanf("%c", &field[rowIndex][columnIndex]);
            }
            scanf("\n");
        }
    }

    MPI_Bcast(&numRows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&numColumns, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&numIterations, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int chunkSize = (numRows + numProcs - 1) / numProcs;
    startIndex = rank * chunkSize;
    endIndex = startIndex + chunkSize;
    if (endIndex > numRows) {
        endIndex = numRows;
    }

    unsigned char piece[chunkSize + 2][NMAX];
    unsigned char nextf[chunkSize][numColumns];

    MPI_Scatter(field[0], chunkSize * NMAX, MPI_UNSIGNED_CHAR,
                piece[1], chunkSize * NMAX, MPI_UNSIGNED_CHAR,
                0, MPI_COMM_WORLD);

    int iter;

    /*
    printf("Task %d, startIndex %d, endIndex %d, chunk size %d\n",
           rank, startIndex, endIndex, chunkSize);
    int i;
    for (i = startIndex; i < endIndex; ++i) {
        int j;
        for (j = 0; j < numColumns; ++j) {
            printf("%c", piece[i - startIndex + 1][j]);
        }
        printf("\n");
    }
    */

    for (iter = 0; iter < numIterations; ++iter) {

        if (rank + 1 == numProcs) {
            MPI_Recv(piece[0], NMAX, MPI_UNSIGNED_CHAR, rank - 1, 1,
                     MPI_COMM_WORLD, &status);
            MPI_Send(piece[endIndex - startIndex], NMAX, MPI_UNSIGNED_CHAR,
                     0, 1, MPI_COMM_WORLD);
        } else {
            MPI_Send(piece[endIndex - startIndex], NMAX, MPI_UNSIGNED_CHAR,
                     rank + 1, 1, MPI_COMM_WORLD);
            MPI_Recv(piece[0], NMAX, MPI_UNSIGNED_CHAR,
                     (rank + numProcs - 1) % numProcs, 1,
                     MPI_COMM_WORLD, &status);
        }

        if (rank + 1 == numProcs) {
            MPI_Recv(piece[endIndex - startIndex + 1], NMAX,
                     MPI_UNSIGNED_CHAR, 0, 1,
                     MPI_COMM_WORLD, &status);
            MPI_Send(piece[1], NMAX, MPI_UNSIGNED_CHAR,
                     rank - 1, 1, MPI_COMM_WORLD);
        } else {
            MPI_Send(piece[1], NMAX, MPI_UNSIGNED_CHAR,
                     (rank + numProcs - 1) % numProcs, 1, MPI_COMM_WORLD);
            MPI_Recv(piece[endIndex - startIndex + 1], NMAX,
                     MPI_UNSIGNED_CHAR, rank + 1, 1,
                     MPI_COMM_WORLD, &status);
        }

        int x, y, i;
        for (x = 0; x < endIndex - startIndex; ++x) {
            for (y = 0; y < numColumns; ++y) {
                int numActiveNeighbours = 0;
                for (i = 0; i < NUM_NEIGHBOURS; ++i) {
                    int xx = x + 1 + dx[i];
                    int yy = (y + dy[i] + numColumns) % numColumns;
                    if (piece[xx][yy] == '1') {
                        ++numActiveNeighbours;
                    }
                }
                if (numActiveNeighbours == 3) {
                    nextf[x][y] = '1';
                } else if (numActiveNeighbours == 2) {
                    nextf[x][y] = piece[x + 1][y];
                } else {
                    nextf[x][y] = '0';
                }
            }
        }
        for (x = 0; x < endIndex - startIndex; ++x) {
            for (y = 0; y < numColumns; ++y) {
                piece[x + 1][y] = nextf[x][y];
            }
        }
    }

    MPI_Gather(piece[1], NMAX * chunkSize, MPI_UNSIGNED_CHAR,
               field[0], NMAX * chunkSize, MPI_UNSIGNED_CHAR,
               0, MPI_COMM_WORLD);

    int i, j;
    if (rank == 0) {
        printf("%d %d\n", numRows, numColumns);
        for (i = 0; i < numRows; ++i) {
            for (j = 0; j < numColumns; ++j) {
                printf("%c", field[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Finalize();

    return 0;
}
