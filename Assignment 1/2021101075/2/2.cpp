#include <bits/stdc++.h>
#include <mpi.h>
#include <fstream>
using namespace std;

void printOutput(vector<long long int> array, long long int N)
{
    for (long long int i = 0; i < N; i++)
    {
        for (long long int j = 0; j < N; j++)
        {
            if (array[i * N + j] == 1e9)
            {
                cout << "-1 ";
            }
            else
            {
                cout << array[i * N + j] << " ";
            }
        }
        cout << "\n";
    }
}

int main(int argc, char *argv[])
{

    // Start time measurement
    MPI_Status status;
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int to_use = size - 1;
    const int MAXLEN = 1001;
    vector<long long int> newArray(MAXLEN * MAXLEN);
    long long int N;

    if (rank == 0)
    {
        cin >> N;
        for (long long i = 0; i < N; i++)
        {
            for (long long j = 0; j < N; j++)
            {
                cin >> newArray[i * N + j];
                if (newArray[i * N + j] == -1)
                    newArray[i * N + j] = 1e9;
            }
        }
    }

    MPI_Bcast(&N, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(newArray.data(), N * N, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();
    if (size > N + 1)
    {
        size = N;
    }
    to_use = size - 1;
    long long int nRowsLocal = N / to_use;
    if (rank == (size - 1))
    {
        nRowsLocal += N % to_use;
    }
    long long int always = N / to_use;

    vector<long long int> currentGrid((nRowsLocal + 2) * N, 0); // Flatten the 2D vector

    for (long long int iter = 0; iter < N; iter++)
    {
        if (rank != 0)
        {
            for (long long int i = (rank - 1) * always; i < nRowsLocal + (rank - 1) * always; i++)
            {
                for (long long int j = 0; j < N; j++)
                {
                    currentGrid[(i - (rank - 1) * always) * N + j] = newArray[i * N + j];
                }
            }

            for (long long int j = 0; j < N; j++)
            {
                currentGrid[nRowsLocal * N + j] = newArray[j * N + iter];
            }
            for (long long int j = 0; j < N; j++)
            {
                currentGrid[(nRowsLocal + 1) * N + j] = newArray[iter * N + j];
            }

            for (long long int i = 0; i < nRowsLocal; i++)
            {
                for (long long int j = 0; j < N; j++)
                {
                    if (currentGrid[(nRowsLocal + 1) * N + j] == 1e9)
                    {
                        continue;
                    }

                    if (currentGrid[i * N + j] > (currentGrid[nRowsLocal * N + i + (rank - 1) * always] + currentGrid[(nRowsLocal + 1) * N + j]))
                    {
                        currentGrid[i * N + j] = currentGrid[nRowsLocal * N + i + (rank - 1) * always] + currentGrid[(nRowsLocal + 1) * N + j];
                    }
                }
            }
            MPI_Send(currentGrid.data(), nRowsLocal * N, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD);
        }
        if (rank == 0)
        {
            for (long long int p = 1; p < size; p++)
            {
                long long int startRow = (p - 1) * always;
                long long int numRows = always;
                if (p == (size - 1))
                {
                    numRows += N % to_use;
                }
                MPI_Recv(newArray.data() + startRow * N, numRows * N, MPI_LONG_LONG_INT, p, 0, MPI_COMM_WORLD, &status);
            }
        }
        MPI_Bcast(newArray.data(), N * N, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();
    if (rank == 0)
    {
        ofstream outFile("performance_data2.txt", ios::app);

        // Append performance information to the file
        outFile << "Processes: " << size << ", ";
        outFile << "Time taken: " << end_time - start_time << " seconds" << endl;

        printOutput(newArray, N);
    }

    MPI_Finalize();
}