#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;

int totalNumbers = 0;
int numberOfPrimes = 0;

vector<int> Listofprimes;
mutex mtx; // Mutex for synchronization

void checkPrime(int start, int end, int threadID) {
    for (int num = start; num <= end; ++num) {
        bool isPrime = true;
        for (int i = 2; i < num; ++i) {
            if (num % i == 0) {
                isPrime = false;
                break;
            }
        }
        if (isPrime) {
            lock_guard<mutex> lock(mtx);
            Listofprimes.push_back(num);
            ++numberOfPrimes;
        }
        {
            lock_guard<mutex> lock(mtx);
            ++totalNumbers;
        }
    }
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    int numThreads = atoi(argv[1]);

    // Read input from file
    ifstream inputFile("in.txt");
    int rangeStart, rangeEnd;
    inputFile >> rangeStart >> rangeEnd;
    inputFile.close();

    // Calculate range for each thread
    int range = (rangeEnd - rangeStart + 1) / numThreads;

    // Create threads
    vector<thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        int start = rangeStart + i * range;
        int end = (i == numThreads - 1) ? rangeEnd : start + range - 1;
        threads.push_back(thread(checkPrime, start, end, i));
    }

    // Join threads
    for (auto& t : threads) {
        t.join();
    }

    // Output results to STDOUT
    {
        lock_guard<mutex> lock(mtx);
        cout << "numOfPrimes=" << numberOfPrimes << ", totalNums=" << totalNumbers << endl;
        cout << "Prime numbers: ";
        for (auto prime : Listofprimes) {
            cout << prime << " ";
        }
        cout << endl;
    }

    return 0;
}