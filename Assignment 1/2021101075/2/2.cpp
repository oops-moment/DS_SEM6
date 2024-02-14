#include <bits/stdc++.h>
#include <mpi.h>
#include <fstream>

using namespace std;

void printMatrix(const vector<long long int> &matrix, long long int N)
{
    // Print the matrix with appropriate formatting
    for (long long int i = 0; i < N; i++)
    {
        for (long long int j = 0; j < N; j++)
        {
            if (matrix[i * N + j] == 1e9)
            {
                cout << "-1 ";
            }
            else
            {
                cout << matrix[i * N + j] << " ";
            }
        }
        cout << "\n";
    }
}

void writeToFile(int size, double time)
{
    ofstream outFile("metrics2.txt", ios::app);
    outFile << "Processes: " << size << ", ";
    outFile << "Time taken: " << time << " seconds" << endl;
    outFile.close();
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
    vector<long long int> adjacencyMatrix(MAXLEN * MAXLEN);
    long long int N;

    if (rank == 0)
    {
        // Read input matrix on rank 0
        cin >> N;
        for (long long i = 0; i < N; i++)
        {
            for (long long j = 0; j < N; j++)
            {
                cin >> adjacencyMatrix[i * N + j];
                if (adjacencyMatrix[i * N + j] == -1)
                {
                    adjacencyMatrix[i * N + j] = 1e9;
                }
            }
        }
    }

    // Broadcast matrix dimensions and data to all processes
    MPI_Bcast(&N, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(adjacencyMatrix.data(), N * N, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    // Perform Floyd-Warshall algorithm if processes are 1

    if (size == 1)
    {
        for (int k = 0; k < N; k++)
        {
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    if (adjacencyMatrix[i * N + k] == 1e9 || adjacencyMatrix[k * N + j] == 1e9)
                    {
                        continue;
                    }
                    if (adjacencyMatrix[i * N + j] > adjacencyMatrix[i * N + k] + adjacencyMatrix[k * N + j])
                    {
                        adjacencyMatrix[i * N + j] = adjacencyMatrix[i * N + k] + adjacencyMatrix[k * N + j];
                    }
                }
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);
        double end_time = MPI_Wtime();
        printMatrix(adjacencyMatrix, N);
        // writeToFile(size, end_time - start_time);
        MPI_Finalize();
        return 0;
    }

    // Adjust size for processes if more than required
    if (size > N + 1)
    {
        size = N;
    }
    to_use = size - 1;
    long long int tempRows = N / to_use;
    if (rank == (size - 1))
    {
        tempRows += N % to_use;
    }
    long long int always = N / to_use;

    vector<long long int> localMatrix((tempRows)*N, 0); // Flatten the 2D vector
    vector<long long int> kth(2 * N, 0);

    for (long long int k = 0; k < N; k++)
    {
        if (rank != 0)
        {
            // Copy the relevant rows to localMatrix for computation
            for (long long int i = (rank - 1) * always; i < tempRows + (rank - 1) * always; i++)
            {
                for (long long int j = 0; j < N; j++)
                {
                    localMatrix[(i - (rank - 1) * always) * N + j] = adjacencyMatrix[i * N + j];
                }
            }

            // Copy the k-th column to localMatrix

            for (long long int j = 0; j < N; j++)
            {
                kth[j] = adjacencyMatrix[j * N + k];
                kth[N + j] = adjacencyMatrix[k * N + j];
            }

            // Perform Floyd-Warshall algorithm locally
            for (long long int i = 0; i < tempRows; i++)
            {
                for (long long int j = 0; j < N; j++)
                {

                    if (kth[N + j] == 1e9)
                    {
                        continue;
                    }
                    long long actual_i = i + (rank - 1) * always;
                    long long distance2 = kth[actual_i] + kth[N + j];
                    if (localMatrix[i * N + j] > distance2)
                    {
                        localMatrix[i * N + j] = distance2;
                    }
                }
            }

            // Send the result back to rank 0
            MPI_Send(localMatrix.data(), tempRows * N, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD);
        }

        if (rank == 0)
        {
            // Receive results from other processes
            for (long long int p = 1; p < size; p++)
            {
                long long int startRow = (p - 1) * always;
                long long int numRows = always;
                if (p == (size - 1))
                {
                    numRows += N % to_use;
                }
                long long totalElements = numRows * N;
                MPI_Recv(&adjacencyMatrix[startRow * N], totalElements, MPI_LONG_LONG_INT, p, 0, MPI_COMM_WORLD, &status);
            }
        }

        // Broadcast the updated matrix to all processes for the next kation
        MPI_Bcast(adjacencyMatrix.data(), N * N, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();

    if (rank == 0)
    {

        // writeToFile(size, end_time - start_time);
        printMatrix(adjacencyMatrix, N);
    }

    MPI_Finalize();
    return 0;
}
