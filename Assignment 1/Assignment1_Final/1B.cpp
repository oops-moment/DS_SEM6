#include <bits/stdc++.h>
#include <cstdlib>
#include <mpi.h>

using namespace std;
bool checkDiagonal(vector<int> perm, long long n)
{
    for (long long j = 0; j < n; j++)
    {
        for (int k = 0; k < n; k++)
        {
            if (k != j)
            {
                if (abs(perm[k] - perm[j]) == abs(k - j))
                {
                    return false;
                }
            }
        }
    }
    return true;
}
void calc_i_permutation(vector<int> &perm, vector<int> fact, long long i, long long n)
{
    for (int b = 0; b < n; b++)
    {
        perm[b] = i / fact[n - 1 - b];
        i = i % fact[n - 1 - b];
    }

    for (int b = n - 1; b > 0; --b)
    {
        for (int a = b - 1; a >= 0; --a)
        {
            if (perm[a] <= perm[b])
            {
                perm[b]++;
            }
        }
    }
}

int nqueens(long long i, long long n, vector<int> fact)
{

    vector<int> perm(n);
    calc_i_permutation(perm, fact, i, n);
    return checkDiagonal(perm, n);
}

int main(int argc, char *argv[])
{

    int rank, size;
    long long i = 0, n = 0, total = 0, subtotal = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0)
    {
        cin >> n;
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    vector<int> pre_fact(n, 1);
    for (int b = 2; b < n; b++)
    {
        pre_fact[b] = b * pre_fact[b - 1];
    }
    long long max = pre_fact[n - 1] * n;
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
        cout << "Total solutions: " << total << endl;
        cout << "Time taken: " << end_time - start_time << endl;
    }
    MPI_Finalize();
    return 0;
}
