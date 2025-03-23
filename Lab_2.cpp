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


void searchNumberMutex(vector<int>& matrix, int start, int end, int& count, int& minNumber, mutex& mtx);
void divisionPartsMutex(vector<int>& matrix, int numberThreads, int& count, int& minNumber, mutex& mtx);
void searchNumberCAS(vector<int>& matrix, int start, int end, atomic<int>& count, atomic<int>& minNumber);
void divisionPartsCAS(vector<int>& matrix, int numberThreads, atomic<int>& count, atomic<int>& minNumber);

void matrixFilling(vector<int>& matrix, int size)
{
    for (int i = 0; i < size; i++)
    {
        matrix[i] = 21 + rand() % 1000;
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

void divisionPartsMutex(vector<int>& matrix, int numberThreads, int& count, int& minNumber, mutex& mtx)
{
    int chunkSize = matrix.size() / numberThreads;
    vector<thread> threads;

    for (int i = 0; i < numberThreads; i++)
    {
        int start = i * chunkSize;
        int end = (i + 1) * chunkSize;

        if (i == numberThreads - 1)
        {
            end = matrix.size();
        }

        threads.emplace_back(searchNumberMutex, ref(matrix), start, end, ref(count), ref(minNumber), ref(mtx));
    }

    for (auto& t : threads)
    {
        t.join();
    }
}

void searchNumberMutex(vector<int>& matrix, int start, int end, int& count, int& minNumber, mutex& mtx)
{
    for (int i = start; i < end; i++)
    {
        if (matrix[i] % 21 == 0)
        {
            mtx.lock();
            count++;
            if (matrix[i] < minNumber)
            {
                minNumber = matrix[i];
            }
            mtx.unlock();
        }
    }
}

void divisionPartsCAS(vector<int>& matrix, int numberThreads, atomic<int>& count, atomic<int>& minNumber) 
{
    int chunkSize = matrix.size() / numberThreads;
    vector<thread> threads;

    for (int i = 0; i < numberThreads; i++) 
    {
        int start = i * chunkSize;
        int end = (i + 1) * chunkSize;

        if (i == numberThreads - 1)
        {
            end = matrix.size();
        }
        threads.emplace_back(searchNumberCAS, ref(matrix), start, end, ref(count), ref(minNumber));
    }
    for (auto& t : threads)
    {
        t.join();
    }
}

void searchNumberCAS(vector<int>& matrix, int start, int end, atomic<int>& count, atomic<int>& minNumber) 
{
    for (int i = start; i < end; i++) 
    {
        if (matrix[i] % 21 == 0) 
        {
            count.fetch_add(1, memory_order_relaxed);
            int oldMin = minNumber.load(memory_order_relaxed);

            while (matrix[i] < oldMin && !minNumber.compare_exchange_weak(oldMin, matrix[i], memory_order_relaxed));
        }
    }
}

int main()
{
    srand(time(0));

    vector<int> sizeMatrix = {100000, 500000, 1000000, 5000000, 30000000, 90000000};
    vector<int> numberThreads = { 1, 4, 8, 16, 32, 64, 128, 256 };

    for (int i = 0; i < sizeMatrix.size(); i++)
    {
        vector<int> matrix(sizeMatrix[i]);

        matrixFilling(matrix, sizeMatrix[i]);

        int count = 0, minNumber = INT_MAX;

        auto start = high_resolution_clock::now();
        searchNumber(matrix, sizeMatrix[i], count, minNumber);

        auto end = high_resolution_clock::now();
        auto duration = duration_cast<nanoseconds>(end - start).count();
        double seconds = duration * 1e-9;

        cout << "\nSize matrix: " << sizeMatrix[i];
        cout << "\n   - Work time: " << fixed << seconds << " seconds" << endl;
        cout << "\n   - Count: " << count << ", Min number: " << minNumber << endl;
    }

    for (int i = 0; i < sizeMatrix.size(); i++)
    {
        vector<int> matrix(sizeMatrix[i]);

        matrixFilling(matrix, sizeMatrix[i]);

        for (int numThreads : numberThreads)
        {
            int count = 0, minNumber = INT_MAX;
            mutex mtx;

            auto start = high_resolution_clock::now();
            divisionPartsMutex(matrix, numThreads, count, minNumber, mtx);
            auto end = high_resolution_clock::now();

            auto duration = duration_cast<nanoseconds>(end - start).count();
            double seconds = duration * 1e-9;

            cout << "\nSize matrix: " << sizeMatrix[i] << " with " << numThreads << " threads";
            cout << "\n   - Work time: " << fixed << seconds << " seconds";
            cout << "\n   - Count: " << count << ", Min number: " << minNumber << endl;
        }
    }

    for (int i = 0; i < sizeMatrix.size(); i++) 
    {
        vector<int> matrix(sizeMatrix[i]);
        matrixFilling(matrix, sizeMatrix[i]);

        for (int numThreads : numberThreads) 
        {
            atomic<int> count(0), minNumber(INT_MAX);

            auto start = high_resolution_clock::now();
            divisionPartsCAS(matrix, numThreads, count, minNumber);
            auto end = high_resolution_clock::now();

            cout << "\nSize matrix: " << sizeMatrix[i] << " with " << numThreads << " threads (CAS)";
            cout << "\n   - Work time: " << duration_cast<nanoseconds>(end - start).count() * 1e-9 << " seconds";
            cout << "\n   - Count: " << count << ", Min number: " << minNumber << endl;
        }
    }

    return 0;
}