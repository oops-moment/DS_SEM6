#include <bits/stdc++.h>
#include "mpi.h"
using namespace std;

int main(int argc, char *argv[])
{
    MPI_Status status;
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int nRows;
    int nCols;
    int iterations;

    if (rank == 0)
    {
        nRows = atoi(argv[1]); // Convert command line argument to an integer
        nCols = atoi(argv[2]);
        iterations = atoi(argv[3]);
        if (nRows < 1 || nCols < 1 || iterations < 1)
        {
            cout << "Invalid Input";
            MPI_Finalize();
            return 0;
        }
    }

    // Broadcast the values to all the processes
    MPI_Bcast(&nRows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&nCols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&iterations, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Number of rows each process is going to take would be N/number of processes, given the last process some extra rows
    int nRowsLocal = nRows / size;
    // Now the thing is if that's the last process, give some extra
    if (rank == (size - 1))
    {
        nRowsLocal += nRows % size;
    }

    // Now the thing is in order to compare neighbors, we need two more rows
    int nrowsLocalWithGhost = nRowsLocal + 2;
    int ncolswithGhost = nCols + 2;

    // Now we need 2 arrays, one for the current and one for the future
    vector<vector<int>> currentGrid(nrowsLocalWithGhost, vector<int>(ncolswithGhost, 0));
    vector<vector<int>> futureGrid(nrowsLocalWithGhost, vector<int>(ncolswithGhost, 0));

    // Let's do it via random number
    for (int i = 1; i <= nRowsLocal; i++)
    {
        for (int j = 1; j <= nCols; j++)
        {
            // // currentGrid[i][j] = ;
            // random number between 0 and 1 complelet random
            currentGrid[i][j] = rand() % 2;
        }
    }

    // Now we would need two neighbors: one is the upper neighbor and the other is the lower neighbor
    int upperNeighbor = (rank == 0) ? size - 1 : rank - 1;
    int lowerNeighbor = (rank == size - 1) ? 0 : rank + 1;

    const int ALIVE = 1;
    const int DEAD = 0;

    // NITERATION LOOP
    for (int iter = 0; iter < iterations; iter++)
    {
        // send top row above
        MPI_Send(&currentGrid[1][0], ncolswithGhost, MPI_INT, upperNeighbor, 0, MPI_COMM_WORLD);

        // send bottom row below
        MPI_Send(&currentGrid[nRowsLocal][0], ncolswithGhost, MPI_INT, lowerNeighbor, 0, MPI_COMM_WORLD);

        // receive the bottom row from below
        MPI_Recv(&currentGrid[nRowsLocal + 1][0], ncolswithGhost, MPI_INT, lowerNeighbor, 0, MPI_COMM_WORLD, &status);

        // receive the top row from above
        MPI_Recv(&currentGrid[0][0], ncolswithGhost, MPI_INT, upperNeighbor, 0, MPI_COMM_WORLD, &status);

        // handling the ghost cols send the first col to the last col and the last col to the first col
        for (int i = 0; i < nrowsLocalWithGhost; i++)
        {
            currentGrid[i][0] = currentGrid[i][nCols];
            currentGrid[i][nCols + 1] = currentGrid[i][1];
        }

        if (rank != 0)
        {
            for (int row = 1; row <= nRowsLocal; row++)
            {
                MPI_Send(&currentGrid[row][1], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
        }
        if (rank == 0)
        {
            cout << "Iteration " << iter << ":\n";
            for (int row = 1; row <= nRowsLocal; row++)
            {
                for (int col = 1; col <= nCols; col++)
                {
                    cout << currentGrid[row][col] << " ";
                }
                cout << endl;
            }

            for (int srcrank = 1; srcrank < size; srcrank++)
            {
                int torecv = nRows / size;
                if (srcrank == size - 1)
                {
                    torecv += nRows % size;
                }
                vector<int> buffer(nCols, 0);
                for (int i = 0; i < torecv; i++)
                {
                    MPI_Recv(buffer.data(), nCols, MPI_INT, srcrank, 0, MPI_COMM_WORLD, &status);
                    for (auto it : buffer)
                    {
                        cout << it << " ";
                    }
                    cout << "\n";
                }
            }
        }

        // display current grid on the screen and update the grid, look through every cell and its 8 neighbors, for each count of live make it dead/alive accordingly
        for (int i = 1; i <= nRowsLocal; i++)
        {
            for (int j = 1; j <= nCols; j++)
            {
                int alive = 0;
                for (int ii = -1; ii <= 1; ii++)
                {
                    for (int jj = -1; jj <= 1; jj++)
                    {
                        if (ii == 0 && jj == 0)
                        {
                            continue;
                        }
                        if (currentGrid[i + ii][j + jj] == ALIVE)
                        {
                            alive++;
                        }
                    }
                }
                if (currentGrid[i][j] == ALIVE)
                {
                    if (alive < 2)
                    {
                        futureGrid[i][j] = DEAD;
                    }
                    else if (alive == 2 || alive == 3)
                    {
                        futureGrid[i][j] = ALIVE;
                    }
                    else if (alive > 3)
                    {
                        futureGrid[i][j] = DEAD;
                    }
                }
                else
                {
                    if (alive == 3)
                    {
                        futureGrid[i][j] = ALIVE;
                    }
                }
            }
        }

        for (int i = 1; i <= nRowsLocal; i++)
        {
            for (int j = 1; j <= nCols; j++)
            {
                currentGrid[i][j] = futureGrid[i][j];
            }
        }
    }

    MPI_Finalize();
}
