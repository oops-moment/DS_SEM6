#include <bits/stdc++.h>
#include "mpi.h"
using namespace std;

int main(int argc, char *argv[])
{
    int rank, processes;
    const int MAXLEN = 20;

    MPI_Status status;
    long long buffer[MAXLEN][MAXLEN];
    long long N = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        cin >> N;
        for (long long i = 0; i < N; i++)
        {
            for (long long j = 0; j < N; j++)
            {
                cin >> buffer[i][j];
                if (buffer[i][j] == -1)
                    buffer[i][j] = 1e9;
            }
        }

        for (int i = 1; i < processes; i++)
            MPI_Send(&N, 1, MPI_LONG_LONG, i, 15, MPI_COMM_WORLD);

        for (int i = 1; i < processes; i++)
            MPI_Send(buffer, N * N, MPI_LONG_LONG, i, 17, MPI_COMM_WORLD);

        long long result[3];
        int disable = 0;
        do
        {
            MPI_Recv(result, 3, MPI_LONG_LONG, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if (status.MPI_TAG == 25)
                disable++;
            else
            {
                buffer[result[1]][result[2]] = min(buffer[result[1]][result[2]], result[0]);

                // Broadcast only the updated part of the buffer
                for (int i = 1; i < processes; i++)
                {
                    MPI_Send(&buffer[result[1]], N, MPI_LONG_LONG, i, 17, MPI_COMM_WORLD);
                }
            }

        } while (disable < processes - 1);

        for (long long i = 0; i < N; i++)
        {
            for (long long j = 0; j < N; j++)
            {
                if (buffer[i][j] == 1e9)
                    cout << "-1"
                         << " ";
                else
                    cout << buffer[i][j] << " ";
            }
            cout << "\n";
        }
        cout << "\n";
    }
    else
    {
        MPI_Recv(&N, 1, MPI_LONG_LONG, 0, 15, MPI_COMM_WORLD, &status);
        MPI_Recv(buffer, N * N, MPI_LONG_LONG, 0, 17, MPI_COMM_WORLD, &status);

        long long remainder = N % (processes - 1);
        long long slices = (N - remainder) / (processes - 1);
        if (rank + 1 != processes)
            remainder = 0;

        long long out[3];
        for (long long k = slices * (rank - 1); k < (slices * (rank - 1) + slices + remainder); k++)
            for (long long i = 0; i < N; ++i)
                for (long long j = 0; j < N; ++j)
                    if ((buffer[i][k] + buffer[k][j] < buffer[i][j]))
                    {
                        buffer[i][j] = buffer[i][k] + buffer[k][j];
                        out[0] = buffer[i][j];
                        out[1] = i;
                        out[2] = j;
                        MPI_Send(out, 3, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
                    }
        int message = 1;
        MPI_Send(&message, 1, MPI_INT, 0, 25, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}
