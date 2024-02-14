#include <iostream>
#include <vector>
#include <fstream>
#include "mpi.h"
using namespace std;

void WriteToFile(int size, double time)
{
    ofstream myfile;
    myfile.open("output.txt", ios::app);
    myfile << size << " " << time << "\n";
    myfile.close();
}

void printMatrix(vector<int> &currentGrid, int nRowsLocal, int nCols, int iter)
{
    for (int i = 1; i <= nRowsLocal; i++)
    {
        for (int j = 1; j <= nCols; j++)
        {
            cout << currentGrid[i * (nCols + 2) + j] << " ";
        }
        cout << "\n";
    }
}
void performIteration(vector<int> &currentGrid, vector<int> &futureGrid, int nRowsLocal, int nCols)
{
    const int ALIVE = 1;
    const int DEAD = 0;

    for (int i = 1; i <= nRowsLocal; i++)
    {
        for (int j = 1; j <= nCols; j++)
        {
            int alive = 0;
            for (int ii = i - 1; ii <= i + 1; ii++)
            {
                for (int jj = j - 1; jj <= j + 1; jj++)
                {
                    if ((ii != i || jj != j) && currentGrid[ii * (nCols + 2) + jj] == ALIVE)
                        alive++;
                }
            }

            if (currentGrid[i * (nCols + 2) + j] == ALIVE)
            {
                if (alive < 2)
                {
                    futureGrid[i * (nCols + 2) + j] = DEAD;
                }
                else if (alive == 2 || alive == 3)
                {
                    futureGrid[i * (nCols + 2) + j] = ALIVE;
                }
                else if (alive > 3)
                {
                    futureGrid[i * (nCols + 2) + j] = DEAD;
                }
            }
            else
            {
                if (alive == 3)
                {
                    futureGrid[i * (nCols + 2) + j] = ALIVE;
                }
            }
        }
    }
}

void updateGrid(vector<int> &currentGrid, vector<int> &futureGrid, int nRowsLocal, int nCols)
{
    for (int i = 1; i <= nRowsLocal; i++)
    {
        for (int j = 1; j <= nCols; j++)
        {
            currentGrid[i * (nCols + 2) + j] = futureGrid[i * (nCols + 2) + j];
        }
    }
}
int main(int argc, char *argv[])
{
    MPI_Status status;
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int MAXLEN = 1001;
    vector<int> newArray(MAXLEN * MAXLEN);
    int nRows;
    int nCols;
    int iterations;

    if (rank == 0)
    {
        cin >> nRows >> nCols >> iterations;
        iterations += 1;
        if (nRows < 2 || nCols < 2 || iterations < 1)
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

    if (size == 1)
    {
        iterations -= 1;
        vector<int> currentGrid1D(((nRows + 2) * (nCols + 2)), 0);
        vector<int> futureGrid1D(((nRows + 2) * (nCols + 2)), 0);

        for (int i = 1; i <= nRows; i++)
        {
            for (int j = 1; j <= nCols; j++)
            {
                currentGrid1D[i * (nCols + 2) + j] = newArray[(i - 1) * nCols + j - 1];
            }
        }

        for (int iter = 0; iter < iterations; iter++)
        {
            performIteration(currentGrid1D, futureGrid1D, nRows, nCols);
            updateGrid(currentGrid1D, futureGrid1D, nRows, nCols);
            if (iter == (iterations - 1))
            {
                printMatrix(currentGrid1D, nRows, nCols, iter);
            }
        }

        double end_time = MPI_Wtime();
        double time_taken = end_time - start_time;

        // Only rank 0 writes to the file
        if (rank == 0)
        {
            // WriteToFile(size, time_taken);
            // cout << "finish"
            // << "\n";
        }

        MPI_Finalize();
        return 0;
    }

    // Broadcast the values to all the processes
    MPI_Bcast(&nRows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&nCols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&iterations, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(newArray.data(), nRows * nCols, MPI_INT, 0, MPI_COMM_WORLD);

    int nRowsLocal = nRows / size;
    if (rank == (size - 1))
    {
        nRowsLocal += nRows % size;
    }

    int nrowsLocalWithGhost = nRowsLocal + 2;
    int ncolswithGhost = nCols + 2;

    vector<int> currentGrid1D(nrowsLocalWithGhost * ncolswithGhost, 0);
    vector<int> futureGrid1D(nrowsLocalWithGhost * ncolswithGhost, 0);

    int up = size - 1;
    if (rank != 0)
    {
        up = rank - 1;
    }
    int down = 0;
    if (rank != (size - 1))
    {
        down = rank + 1;
    }

    for (int i = 1 + rank * (nRows / size); i <= nRowsLocal + rank * (nRows / size); i++)
    {
        for (int j = 1; j <= nCols; j++)
        {
            int indexx1 = (i - rank * (nRows / size)) * (ncolswithGhost) + j;
            int indexx2 = (i - 1) * nCols + (j - 1);

            currentGrid1D[indexx1] = newArray[indexx2];
        }
    }
    // NITERATION LOOP
    for (int iter = 0; iter < iterations; iter++)
    {
        MPI_Send(&currentGrid1D[ncolswithGhost], ncolswithGhost, MPI_INT, up, 0, MPI_COMM_WORLD);
        MPI_Send(&currentGrid1D[nRowsLocal * ncolswithGhost], ncolswithGhost, MPI_INT, down, 0, MPI_COMM_WORLD);

        MPI_Recv(&currentGrid1D[(nRowsLocal + 1) * ncolswithGhost], ncolswithGhost, MPI_INT, down, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&currentGrid1D[0], ncolswithGhost, MPI_INT, up, 0, MPI_COMM_WORLD, &status);

        if (rank == 0)
        {
            for (int i = 0; i < ncolswithGhost; i++)
            {
                currentGrid1D[i] = 0;
            }
        }
        if (rank == size - 1)
        {
            for (int i = 0; i < ncolswithGhost; i++)
            {
                currentGrid1D[(nRowsLocal + 1) * ncolswithGhost + i] = 0;
            }
        }

        if (rank != 0)
        {
            for (int row = 1; row <= nRowsLocal; row++)
            {
                MPI_Send(&currentGrid1D[row * ncolswithGhost + 1], nCols, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
        }
        if (rank == 0)
        {
            if (iter == (iterations - 1))
            {
                printMatrix(currentGrid1D, nRowsLocal, nCols, iter);
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
                    if (iter == (iterations - 1))
                    {
                        for (auto it : buffer)
                        {
                            cout << it << " ";
                        }
                        cout << "\n";
                    }
                }
            }
        }
        performIteration(currentGrid1D, futureGrid1D, nRowsLocal, nCols);
        updateGrid(currentGrid1D, futureGrid1D, nRowsLocal, nCols);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();

    if (rank == 0)
    {
        // WriteToFile(size, end_time - start_time);
    }

    MPI_Finalize();
}
