#include "mpi.h"
#include <iostream>
#include <cstring>
using namespace std;

int main(int argc, char *argv[])
{
    const int MAXLEN = 256;
    int rank, size, tag = 0;
    MPI_Status stat;
    int buffer[MAXLEN][MAXLEN];
    int len = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Check that all processors are alive
    cout << "Processor " << rank << " of " << size << " is alive" << endl;

    MPI_Barrier(MPI_COMM_WORLD);

    // Send and receive some information; send the length first
    if (rank == 0)
    {
        cin >> len;
        for (int i = 0; i < len; i++)
        {
            for (int j = 0; j < len; j++)
            {
                cin >> buffer[i][j];
            }
        }
        MPI_Send(&len, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(buffer, len * len, MPI_CHAR, 1, 1, MPI_COMM_WORLD);
        MPI_Recv(buffer, len * len, MPI_CHAR, 1, 2, MPI_COMM_WORLD, &stat);
        for (int i = 0; i < len; i++)
        {
            for (int j = 0; j < len; j++)
            {
                cout << buffer[i][j] << " ";
            }
            cout << endl;
        }
    }
    else if (rank == 1)
    {
        MPI_Recv(&len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);
        MPI_Recv(buffer, len * len, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &stat);
        buffer[0][0] = 2;
        MPI_Send(buffer, len * len, MPI_CHAR, 0, 2, MPI_COMM_WORLD);
    }

    MPI_Finalize();
}