#include <bits/stdc++.h>
using namespace std;

class Solution
{
public:
    void shortest_distance(vector<vector<int>> &matrix)
    {
        int n = matrix.size();
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (matrix[i][j] == -1)
                {
                    matrix[i][j] = 1e9;
                }
                if (i == j)
                    matrix[i][j] = 0;
            }
        }

        for (int k = 0; k < n; k++)
        {
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    matrix[i][j] = min(matrix[i][j],
                                       matrix[i][k] + matrix[k][j]);
                }
            }
        }

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (matrix[i][j] == 1e9)
                {
                    matrix[i][j] = -1;
                }
            }
        }
    }
};

int main()
{

    int V = 5;
    vector<vector<int>> matrix(V, vector<int>(V, -1));
    matrix[0][0] = 0;
    matrix[0][1] = 4;
    matrix[0][2] = -1;
    matrix[0][3] = 5;
    matrix[0][4] = -1;
    matrix[1][0] = -1;
    matrix[1][1] = 0;
    matrix[1][2] = 1;
    matrix[1][3] = -1;
    matrix[1][4] = 6;
    matrix[2][0] = 2;
    matrix[2][1] = -1;
    matrix[2][2] = 0;
    matrix[2][3] = 3;
    matrix[2][4] = -1;
    matrix[3][0] = -1;
    matrix[3][1] = -1;
    matrix[3][2] = 1;
    matrix[3][3] = 0;
    matrix[3][4] = 2;
    matrix[4][0] = 1;
    matrix[4][1] = -1;
    matrix[4][2] = -1;
    matrix[4][3] = 4;
    matrix[4][4] = 0;

    Solution obj;
    obj.shortest_distance(matrix);

    for (auto row : matrix)
    {
        for (auto cell : row)
        {
            cout << cell << " ";
        }
        cout << endl;
    }

    return 0;
}