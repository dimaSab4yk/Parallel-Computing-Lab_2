#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <vector>
#include <ctime>
#include <climits>

using namespace std;
using namespace std::chrono;

void matrixFilling(vector<int>& matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        matrix[i] = rand() % 1000;
    }
}

void searchNumber(vector<int>& matrix, int size, int& count, int& minNumber)
{
    for (int i = 0; i < size; i++)
    {
        if (matrix[i] % 21 == 0)
        {
            count++;

            if (matrix[i] < minNumber)
            {
                minNumber = matrix[i];
            }
        }
    }
}

int main()
{
    srand(time(0));

    vector<int> sizeMatrix = {100000, 500000, 1000000, 5000000, 30000000};

    for (int i = 0; i < sizeMatrix.size(); i++)
    {
        vector<int> matrix(sizeMatrix[i]);

        matrixFilling(matrix, sizeMatrix[i]);

        int count = 0, minNumber = INT_MAX;
        cout << endl;

        auto start = high_resolution_clock::now();
        searchNumber(matrix, sizeMatrix[i], count, minNumber);

        auto end = high_resolution_clock::now();
        auto duration = duration_cast<nanoseconds>(end - start).count();
        double seconds = duration * 1e-9;

        cout << "\nSize matrix: " << sizeMatrix[i];
        cout << "\n   - Work time: " << fixed << seconds << " seconds" << endl;
    }
}


