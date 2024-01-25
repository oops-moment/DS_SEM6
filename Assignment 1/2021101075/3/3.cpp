#include <iostream>
#include <vector>
#include <fstream>
#include "mpi.h"
using namespace std;

int main(int argc, char *argv[])
{
    MPI_Status status;
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int MAXLEN = 101;
    vector<int> newArray(MAXLEN * MAXLEN);
    int nRows;
    int nCols;
    int iterations;

    if (rank == 0)
    {
        cin >> nRows >> nCols >> iterations;
        iterations += 1;
        if (nRows < 1 || nCols < 1 || iterations < 1)
        {
            cout << "Invalid Input";
            MPI_Finalize();
            return 0;
        }
        for (long long i = 0; i < nRows; i++)
        {
            for (long long j = 0; j < nCols; j++)
            {
                cin >> newArray[i * nCols + j];
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    // Broadcast the values to all the processes
    MPI_Bcast(&nRows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&nCols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&iterations, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(newArray.data(), nRows * nCols, MPI_INT, 0, MPI_COMM_WORLD);

    // Number of rows each process is going to take would be N/number of processes, given the last process some extra rows

    int nRowsLocal = nRows / size;

    // Now the thing is if that's the last process, give some extra
    if (rank == (size - 1))
    {
        nRowsLocal += nRows % size;
    }

    int nrowsLocalWithGhost = nRowsLocal + 2; // 12
    int ncolswithGhost = nCols + 2;           // 12

    // Now we need 2 arrays, one for the current and one for the future
    vector<vector<int>> currentGrid(nrowsLocalWithGhost, vector<int>(ncolswithGhost, 0)); // 12X12
    vector<vector<int>> futureGrid(nrowsLocalWithGhost, vector<int>(ncolswithGhost, 0));  // 12X12

    for (int i = 1 + rank * (nRows / size); i <= nRowsLocal + rank * (nRows / size); i++)
    {
        for (int j = 1; j <= nCols; j++)
        {
            currentGrid[i - rank * (nRows / size)][j] = newArray[(i - 1) * nCols + j - 1];
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

        if (rank == 0)
        {
            for (int i = 0; i < ncolswithGhost; i++)
            {
                currentGrid[0][i] = 0;
            }
        }
        if (rank == size - 1)
        {
            for (int i = 0; i < ncolswithGhost; i++)
            {
                currentGrid[nRowsLocal + 1][i] = 0;
            }
        }

        if (rank != 0)
        {
            for (int row = 1; row <= nRowsLocal; row++)
            {
                MPI_Send(&currentGrid[row][1], nCols, MPI_INT, 0, 0, MPI_COMM_WORLD);
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

        for (int i = 1; i <= nRowsLocal; i++)
        {
            for (int j = 1; j <= nCols; j++)
            {
                int alive = 0;
                for (int ii = i - 1; ii <= i + 1; ii++)
                {
                    for (int jj = j - 1; jj <= j + 1; jj++)
                    {
                        if ((ii != i || jj != j) && currentGrid[ii][jj] == ALIVE)
                            alive++;
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

    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();

    if (rank == 0)
    {
        ofstream outFile("performance_data3.txt", ios::app);
        // Append performance information to the file
        outFile << "Processes: " << size << ", ";
        outFile << "Time taken: " << end_time - start_time << " seconds" << endl;
        cout << "finish"
             << "\n";
    }

    MPI_Finalize();
}
