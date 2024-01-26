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

void printMatrix(vector<vector<int>> &currentGrid, int nRowsLocal, int nCols, int iter)
{
    cout << "Iteration " << iter << "\n";
    for (int i = 1; i <= nRowsLocal; i++)
    {
        for (int j = 1; j <= nCols; j++)
        {
            cout << currentGrid[i][j] << " ";
        }
        cout << "\n";
    }
}
void performIteration(vector<vector<int>> &currentGrid, vector<vector<int>> &futureGrid, int nRowsLocal, int nCols)
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
}

void updateGrid(vector<vector<int>> &currentGrid, vector<vector<int>> &futureGrid, int nRowsLocal, int nCols)
{
    for (int i = 1; i <= nRowsLocal; i++)
    {
        for (int j = 1; j <= nCols; j++)
        {
            currentGrid[i][j] = futureGrid[i][j];
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
        vector<vector<int>> currentGrid(nRows + 2, vector<int>(nCols + 2, 0));
        vector<vector<int>> futureGrid(nRows + 2, vector<int>(nCols + 2, 0));

        for (int i = 1; i <= nRows; i++)
        {
            for (int j = 1; j <= nCols; j++)
            {
                currentGrid[i][j] = newArray[(i - 1) * nCols + j - 1];
            }
        }

        for (int iter = 0; iter < iterations; iter++)
        {
            performIteration(currentGrid, futureGrid, nRows, nCols);
            updateGrid(currentGrid, futureGrid, nRows, nCols);
            printMatrix(currentGrid, nRows, nCols, iter);
        }

        double end_time = MPI_Wtime();
        double time_taken = end_time - start_time;

        // Only rank 0 writes to the file
        if (rank == 0)
        {
            WriteToFile(size, time_taken);
            cout << "finish"
                 << "\n";
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

    vector<vector<int>> currentGrid(nrowsLocalWithGhost, vector<int>(ncolswithGhost, 0));
    vector<vector<int>> futureGrid(nrowsLocalWithGhost, vector<int>(ncolswithGhost, 0));

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
            int roww = i - rank * (nRows / size);
            int indexx = (i - 1) * nCols + (j - 1);

            currentGrid[roww][j] = newArray[indexx];
        }
    }
    // NITERATION LOOP
    for (int iter = 0; iter < iterations; iter++)
    {
        MPI_Send(&currentGrid[1][0], ncolswithGhost, MPI_INT, up, 0, MPI_COMM_WORLD);
        MPI_Send(&currentGrid[nRowsLocal][0], ncolswithGhost, MPI_INT, down, 0, MPI_COMM_WORLD);

        MPI_Recv(&currentGrid[nRowsLocal + 1][0], ncolswithGhost, MPI_INT, down, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&currentGrid[0][0], ncolswithGhost, MPI_INT, up, 0, MPI_COMM_WORLD, &status);

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

            printMatrix(currentGrid, nRowsLocal, nCols, iter);
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
        performIteration(currentGrid, futureGrid, nRowsLocal, nCols);
        updateGrid(currentGrid, futureGrid, nRowsLocal, nCols);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();

    if (rank == 0)
    {
        WriteToFile(size, end_time - start_time);
        cout << "finish"
             << "\n";
    }

    MPI_Finalize();
}
