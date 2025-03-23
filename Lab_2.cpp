#include <iostream>
#include <cstdlib>
#include <climits>
#include <chrono>
#include <thread>
#include <vector>
#include <ctime>
#include <mutex>

using namespace std;
using namespace std::chrono;

void searchNumber(vector<int>& matrix, int start, int end, int& localCount, int& localMin);
void divisionParts(vector<int>& matrix, int numberThreads, int& count, int& minNumber);

void matrixFilling(vector<int>& matrix)
{
    for (int& num : matrix)
    {
        num = rand() % 1000;
    }
}

void searchNumber(vector<int>& matrix, int start, int end, int& localCount, int& localMin)
{
    for (int i = start; i < end; i++)
    {
        if (matrix[i] % 21 == 0)
        {
            localCount++;
            if (matrix[i] < localMin)
            {
                localMin = matrix[i];
            }
        }
    }
}

void divisionParts(vector<int>& matrix, int numberThreads, int& count, int& minNumber)
{
    int chunkSize = matrix.size() / numberThreads;
    vector<thread> threads;
    vector<int> localCounts(numberThreads, 0);
    vector<int> localMins(numberThreads, INT_MAX);

    for (int i = 0; i < numberThreads; i++)
    {
        int start = i * chunkSize;
        int end = (i == numberThreads - 1) ? matrix.size() : (i + 1) * chunkSize;

        threads.emplace_back(searchNumber, ref(matrix), start, end, ref(localCounts[i]), ref(localMins[i]));
    }

    for (auto& t : threads)
    {
        t.join();
    }

    count = 0;
    minNumber = INT_MAX;
    for (int i = 0; i < numberThreads; i++)
    {
        count += localCounts[i];
        if (localMins[i] < minNumber)
        {
            minNumber = localMins[i];
        }
    }
}

int main()
{
    srand(time(0));

    vector<int> sizeMatrix = { 100000, 500000, 1000000, 5000000, 30000000, 90000000 };
    vector<int> numberThreads = { 1, 4, 8, 16, 32, 64, 128, 256 };

    for (int size : sizeMatrix)
    {
        vector<int> matrix(size);
        matrixFilling(matrix);

        int count = 0, minNumber = INT_MAX;

        auto start = high_resolution_clock::now();
        searchNumber(matrix, 0, size, count, minNumber);
        auto end = high_resolution_clock::now();

        auto duration = duration_cast<nanoseconds>(end - start).count();
        double seconds = duration * 1e-9;

        cout << "\nSize matrix: " << size;
        cout << "\n   - Work time: " << fixed << seconds << " seconds";
        if (count > 0)
            cout << "\n   - Count: " << count << ", Min number: " << minNumber << endl;
        else
            cout << "\n   - Count: " << count << ", No numbers divisible by 21 found." << endl;
    }

    for (int size : sizeMatrix)
    {
        vector<int> matrix(size);
        matrixFilling(matrix);

        for (int numThreads : numberThreads)
        {
            int count = 0, minNumber = INT_MAX;

            auto start = high_resolution_clock::now();
            divisionParts(matrix, numThreads, count, minNumber);
            auto end = high_resolution_clock::now();

            auto duration = duration_cast<nanoseconds>(end - start).count();
            double seconds = duration * 1e-9;

            cout << "\nSize matrix: " << size << " with " << numThreads << " threads";
            cout << "\n   - Work time: " << fixed << seconds << " seconds";
            if (count > 0)
                cout << "\n   - Count: " << count << ", Min number: " << minNumber << endl;
            else
                cout << "\n   - Count: " << count << ", No numbers divisible by 21 found." << endl;
        }
    }

    return 0;
}
