#include <iostream>
using namespace std;

// change row and column value to set the canvas size
const int row = 5;
const int col = 5;

// creates row boundary
int row_line()
{
    cout << endl;
    for (int i = 0; i < col; i++)
    {
        cout << " -----";
    }
    cout << endl;
}

// returns the count of alive neighbours
int count_live_neighbour_cell(int a[row][col], int r, int c)
{
    int i, j, count = 0;
    for (i = r - 1; i <= r + 1; i++)
    {
        for (j = c - 1; j <= c + 1; j++)
        {
            if ((i == r && j == c) || (i < 0 || j < 0) || (i >= row || j >= col))
            {
                continue;
            }
            if (a[i][j] == 1)
            {
                count++;
            }
        }
    }
    return count;
}

int main()
{
    int a[row][col], b[row][col];
    int i, j;
    int neighbour_live_cell;

    // take input for the matrix from the user
    cout << "Enter the elements of the matrix (0 or 1):\n";
    for (i = 0; i < row; i++)
    {
        for (j = 0; j < col; j++)
        {
            cin >> a[i][j];
        }
    }

    // print array matrix
    cout << "Initial Stage:";
    row_line();
    for (i = 0; i < row; i++)
    {
        cout << ":";
        for (j = 0; j < col; j++)
        {
            cout << " " << a[i][j] << " :";
        }
        row_line();
    }

    // next canvas values based on live neighbour count
    for (i = 0; i < row; i++)
    {
        for (j = 0; j < col; j++)
        {
            neighbour_live_cell = count_live_neighbour_cell(a, i, j);
            if (a[i][j] == 1 && (neighbour_live_cell == 2 || neighbour_live_cell == 3))
            {
                b[i][j] = 1;
            }
            else if (a[i][j] == 0 && neighbour_live_cell == 3)
            {
                b[i][j] = 1;
            }
            else
            {
                b[i][j] = 0;
            }
        }
    }

    // print next generation
    cout << "\nNext Generation:";
    row_line();
    for (i = 0; i < row; i++)
    {
        cout << ":";
        for (j = 0; j < col; j++)
        {
            cout << " " << b[i][j] << " :";
        }
        row_line();
    }

    return 0;
}
