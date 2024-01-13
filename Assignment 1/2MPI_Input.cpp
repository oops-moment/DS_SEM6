#include <bits/stdc++.h>
#include "mpi.h"
using namespace std;

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
        cin >> N;
        vector<vector<int>> vec(N, vector<int>(N));

        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                cin >> vec[i][j];
                if (vec[i][j] == -1)
                    vec[i][j] = 0;
            }
        }

        for (int i = 1; i < processes; i++)
        {
            MPI_Send(&N, 1, MPI_INT, i, 0, MPI_COMM_WORLD);             // Send N to other processes
            MPI_Send(&vec[0][0], N * N, MPI_INT, i, 1, MPI_COMM_WORLD); // Send vector to other processes
        }
        vector<int> result(3);
        int disable = 0;
        int expectedTerminationMessages = processes - 1;

        do
        {
            MPI_Recv(&result[0], 3, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == 2)
            {
                disable++;
                if (disable == expectedTerminationMessages)
                    break;
            }
            else
            {
                if (vec[result[1]][result[2]] > result[0])
                    vec[result[1]][result[2]] = result[0];
            }
        } while (true);

        for (int i = 0; i < vec.size(); i++)
        {
            for (int j = 0; j < vec.size(); j++)
            {
                if (vec[i][j] == 0 && i != j)
                    cout << "-1"
                         << " ";
                else
                    cout << vec[i][j] << " ";
            }
            cout << "\n";
        }
        cout << "\n";
    }
    else
    {
        int n;

        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status); // Receive N from root process
        vector<vector<int>> vecc(n, vector<int>(n));
        MPI_Recv(&vecc[0][0], n * n, MPI_INT, 0, 1, MPI_COMM_WORLD, &status); // Receive vector from root process
        int remainder = 0;
        if (processes > 1)
        {
            remainder = n % (processes - 1);
        }
        int slices = (n - remainder) / (processes - 1);
        if (rank + 1 != processes)
            remainder = 0;

        vector<int> out(3);

        for (int k = slices * (rank - 1); k < (slices * (rank - 1) + slices + remainder); k++)
        {
            for (int i = 0; i < n; ++i)
            {
                for (int j = 0; j < n; ++j)
                {

                    if (vecc[i][k] * vecc[k][j] != 0 && (i != j))
                    {
                        if ((vecc[i][k] + vecc[k][j] < vecc[i][j]) || (vecc[i][j] == 0))
                        {
                            vecc[i][j] = vecc[i][k] + vecc[k][j];
                            out[0] = vecc[i][j];
                            out[1] = i;
                            out[2] = j;
                            MPI_Send(&out[0], 3, MPI_INT, 0, 0, MPI_COMM_WORLD);
                        }
                    }
                }
            }
        }
        int message = 0;
        MPI_Send(&message, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}
