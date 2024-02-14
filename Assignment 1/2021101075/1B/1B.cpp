#include <bits/stdc++.h>
#include <cstdlib>
#include <mpi.h>
#include <fstream>

using namespace std;

bool checkDiagonal(const vector<int> &permutation, long long n)
{
    // Check if queens in the permutation attack each other diagonally
    for (long long j = 0; j < n; j++)
    {
        for (int k = 0; k < n; k++)
        {
            if (k != j)
            {
                if (abs(permutation[k] - permutation[j]) == abs(k - j))
                {
                    return false; // Queens attack each other
                }
            }
        }
    }
    return true; // No diagonal attacks
}

void calculateIthPermutation(vector<int> &perm, const vector<int> &fact, long long i, long long n)
{
    // Calculate the i-th lexicographically smallest permutation
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

void writePerformanceToFile(double time, int total, int size)
{
    // Write performance data to a file
    ofstream outFile("metrics1.txt", ios::app);
    outFile << "Processes: " << size << ", ";
    outFile << "Total solutions: " << total << ", ";
    outFile << "Time taken: " << time << " seconds" << endl;
    outFile.close();
}

void calculateFactorials(vector<int> &pre_fact, int n)
{
    // Calculate factorials up to n and store in pre_fact vector
    for (int b = 2; b < n; b++)
    {
        pre_fact[b] = b * pre_fact[b - 1];
    }
}

int nQueens(long long i, long long n, const vector<int> &fact)
{
    // Count the number of solutions for the i-th permutation
    vector<int> perm(n);
    calculateIthPermutation(perm, fact, i, n);
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
        // cout << "Enter the number of queens: ";
        cin >> n;
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    vector<int> pre_fact(n, 1);
    calculateFactorials(pre_fact, n);

    long long max = pre_fact[n - 1] * n;
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    for (i = rank; i < max; i += size)
    {
        subtotal += nQueens(i, n, pre_fact);
    }

    MPI_Reduce(&subtotal, &total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    if (rank == 0)
    {
        // writePerformanceToFile(end_time - start_time, total, size);
        // cout << "Total solutions: " << total << endl;
        cout << total << "\n";
    }

    MPI_Finalize();
    return 0;
}
