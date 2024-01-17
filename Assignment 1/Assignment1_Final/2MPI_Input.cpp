#include <bits/stdc++.h>
#include "mpi.h"
using namespace std;

void printVector(vector<int> vec)
{
    int N = sqrt(vec.size());
    for (int i = 0; i < vec.size(); i++)
    {
        if (vec[i] == 1e9)
            cout << "-1"
                 << " ";
        else
            cout << vec[i] << " ";

        if ((i + 1) % N == 0)
            cout << "\n";
    }
}

int main(int argc, char *argv[])
{
    int rank, processes;
    int N;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        double start, end;
        start = MPI_Wtime();
        cin >> N;
        MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
        vector<int> vec(N * N);
        for (int i = 0; i < (N * N); i++)
        {
            cin >> vec[i];
            if (vec[i] == -1)
            {
                vec[i] = 1e9;
            }
        }

        if (processes == 1)
        {
            for (int k = 0; k < N; k++)
            {
                for (int i = 0; i < N; i++)
                {
                    for (int j = 0; j < N; j++)
                    {
                        if ((vec[i * N + k] + vec[k * N + j] < vec[i * N + j]))
                        {
                            vec[i * N + j] = vec[i * N + k] + vec[k * N + j];
                        }
                    }
                }
            }
            end = MPI_Wtime();
            cout << "\n";
            cout << "Time taken: " << end - start << "\n";
            cout << " Final Result is \n";
            printVector(vec);
            MPI_Finalize();
            return 0;
        }
        int size = vec.size();
        MPI_Bcast(vec.data(), size, MPI_INT, 0, MPI_COMM_WORLD);
        vector<int> result(3);
        int total_recvs = processes - 1;
        int disable = 0;
        do
        {
            MPI_Recv(result.data(), 3, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if (status.MPI_TAG == 2)
                disable++;
            else
                vec[result[1] * N + result[2]] = min(vec[result[1] * N + result[2]], result[0]);
        } while (disable < total_recvs);
        end = MPI_Wtime();
        cout << "\n";
        cout << "Time taken: " << end - start << "\n";
        cout << " Final Result is \n";
        printVector(vec);
    }
    else
    {
        MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
        vector<int> vec(N * N);
        MPI_Bcast(vec.data(), N * N, MPI_INT, 0, MPI_COMM_WORLD);

        int remainder = N % (processes - 1);
        int slices = (N - remainder) / (processes - 1);
        if ((rank + 1) != processes)
            remainder = 0;

        // only the last process does the extra job that has to be done

        vector<int> out(3);

        for (int k = (slices * (rank - 1)); k < ((slices * (rank - 1)) + slices + remainder); k++)
        {
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    if ((vec[i * N + k] + vec[k * N + j] < vec[i * N + j]))
                    {
                        vec[i * N + j] = vec[i * N + k] + vec[k * N + j];
                        out[0] = vec[i * N + j];
                        out[1] = i;
                        out[2] = j;
                        MPI_Send(out.data(), 3, MPI_INT, 0, 0, MPI_COMM_WORLD);
                    }
                }
            }
        }
        MPI_Send(out.data(), 3, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
