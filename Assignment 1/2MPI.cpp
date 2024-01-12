#include <bits/stdc++.h>
#include "mpi.h"
using namespace std;

void printvec(vector<vector<int>> vec)
{
    for (int i = 0; i < vec.size(); i++)
    {
        for (int j = 0; j < vec.size(); j++)
        {
            if (vec[i][j] == 1e9)
                cout << "-1"
                     << " ";
            else
                cout << vec[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

int main(int argc, char *argv[])
{
    int rank, processes;
    int N = 4; // Initialize N before using it
    vector<vector<int>> vec(N, vector<int>(N, 0));
    vec[0][0] = 0;
    vec[0][1] = 2;
    vec[0][2] = 1e9;
    vec[0][3] = 1e9;
    vec[1][0] = 1;
    vec[1][1] = 0;
    vec[1][2] = 3;
    vec[1][3] = 1e9;
    vec[2][0] = 1e9;
    vec[2][1] = 1e9;
    vec[2][2] = 0;
    vec[2][3] = 1e9;
    vec[3][0] = 3;
    vec[3][1] = 5;
    vec[3][2] = 4;
    vec[3][3] = 0;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        for (int i = 1; i < processes; i++)
            MPI_Send(&vec[0][0], N * N, MPI_INT, i, 1, MPI_COMM_WORLD);

        vector<int> result(3);
        int disable = 0;
        do
        {
            MPI_Recv(&result[0], 3, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if (status.MPI_TAG == 2)
                disable++;
            else
                vec[result[1]][result[2]] = min(vec[result[1]][result[2]], result[0]);
        } while (disable < processes - 1);

        printvec(vec);
    }
    else
    {
        MPI_Recv(&vec[0][0], N * N, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        int remainder = N % (processes - 1);
        int slices = (N - remainder) / (processes - 1);
        if (rank + 1 != processes)
            remainder = 0;

        vector<int> out(3);

        for (int k = slices * (rank - 1); k < (slices * (rank - 1) + slices + remainder); k++)
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    if ((vec[i][k] + vec[k][j] < vec[i][j]) || (vec[i][j] == 0))
                    {
                        vec[i][j] = vec[i][k] + vec[k][j];
                        out[0] = vec[i][j];
                        out[1] = i;
                        out[2] = j;
                        MPI_Send(&out[0], 3, MPI_INT, 0, 0, MPI_COMM_WORLD);
                    }
        MPI_Send(&out[0], 3, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}
