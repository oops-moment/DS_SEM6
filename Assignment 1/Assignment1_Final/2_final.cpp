#include <bits/stdc++.h>
#include <mpi.h>
using namespace std;

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
    vector<int> newArray(MAXLEN * MAXLEN);
    int N;
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

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(newArray.data(), N * N, MPI_INT, 0, MPI_COMM_WORLD);

    int nRowsLocal = N / size;
    if (rank == (size - 1))
    {
        nRowsLocal += N % to_use;
    }
    int always = N / to_use;

    vector<int> currentGrid((nRowsLocal + 2) * N, 0); // Flatten the 2D vector

    for (int iter = 0; iter < N; iter++)
    {
        if (rank != 0)
        {
            for (int i = (rank - 1) * always; i < nRowsLocal + (rank - 1) * always; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    currentGrid[(i - (rank - 1) * always) * N + j] = newArray[i * N + j];
                }
            }

            for (int j = 0; j < N; j++)
            {
                currentGrid[nRowsLocal * N + j] = newArray[j * N + iter];
            }
            for (int j = 0; j < N; j++)
            {
                currentGrid[(nRowsLocal + 1) * N + j] = newArray[iter * N + j];
            }

            for (int i = 0; i < nRowsLocal; i++)
            {
                for (int j = 0; j < N; j++)
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
            MPI_Send(currentGrid.data(), nRowsLocal * N, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
        if (rank == 0)
        {
            for (int p = 1; p < size; p++)
            {
                int startRow = (p - 1) * always;
                int numRows = always;
                if (p == (size - 1))
                {
                    numRows += N % to_use;
                }
                MPI_Recv(newArray.data() + startRow * N, numRows * N, MPI_INT, p, 0, MPI_COMM_WORLD, &status);
            }
        }
        MPI_Bcast(newArray.data(), N * N, MPI_INT, 0, MPI_COMM_WORLD);
    }

    if (rank == 0)
    {
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (newArray[i * N + j] == 1e9)
                    cout << -1 << " ";
                else
                    cout << newArray[i * N + j] << " ";
            }
            cout << endl;
        }
    }

    MPI_Finalize();
}