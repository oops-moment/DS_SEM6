#include <bits/stdc++.h>
#include <cstdlib>
#include <mpi.h>

using namespace std;

long long factorial(long long n)
{
    if (n == 0 || n == 1)
    {
        return 1;
    }
    else
    {
        return n * factorial(n - 1);
    }
}

int nqueens(long long i, long long n, vector<int> fact)
{

    int a, b = 0;

    vector<int> perm(n);
    for (b = 0; b < n; b++)
    {
        perm[b] = i / fact[n - 1 - b];
        i = i % fact[n - 1 - b];
    }

    for (b = n - 1; b > 0; --b)
    {
        for (a = b - 1; a >= 0; --a)
        {
            if (perm[a] <= perm[b])
            {
                perm[b]++;
            }
        }
    }

    for (long long j = 0; j < n; j++)
    {
        for (int k = 0; k < n; k++)
        {
            if (k != j)
            {
                if (abs(perm[k] - perm[j]) == abs(k - j))
                {
                    return 0;
                }
            }
        }
    }
    return 1;
}

int main(int argc, char *argv[])
{

    int rank, size;
    long long i = 0, n = 0, total = 0, subtotal = 0;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    n = atoi(argv[1]);
    long long max = factorial(n);

    vector<int> pre_fact(n);
    for (int b = 0; b < n; b++)
    {
        pre_fact[b] = factorial(b);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    for (i = rank; i < max; i += size)
    {
        subtotal += nqueens(i, n, pre_fact);
    }
    MPI_Reduce(&subtotal, &total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();
    if (rank == 0)
    {
        ofstream file("matrix.txt", ios::app); // Open file in append mode
        if (file.is_open())
        {
            file << "Number of Processes: " << size << "\t";
            file << "Time taken: " << end_time - start_time << " seconds" << endl;
            file.close();
        }
        else
        {
            cout << "Unable to open file" << endl;
        }
        cout << "Total solutions: " << total << endl;
        cout << "Time taken: " << end_time - start_time << endl;
    }

    MPI_Finalize();
    return 0;
}
