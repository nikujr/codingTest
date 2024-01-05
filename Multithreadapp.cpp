#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <unordered_set>
#include <algorithm>

using namespace std;

// Mutex to synchronize access to the global unique number list
mutex mtx;

// Global unique number list
unordered_set<int> uniqueNumbers;

// Function to process a section of the file by a thread
void processSection(const string& filename, int start, int end) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Unable to open file: " << filename << endl;
        return;
    }

    // Move file cursor to the start position of the section
    file.seekg(start);

    int num;
    while (file >> num && file.tellg() <= end) {
        // Lock the mutex before accessing the global unique number list
        mtx.lock();

        // Add the unique number to the global list
        if (uniqueNumbers.find(num) == uniqueNumbers.end()) {
            uniqueNumbers.insert(num);
        }

        // Unlock the mutex after modifying the global list
        mtx.unlock();
    }

    file.close();
}

int main() {
    const string filename = "ABC.txt"; // Replace with your file name
    const int numThreads = 4; // Number of threads to process the file
    const int fileSize = 10000; // Size of the file (in bytes), change accordingly

    vector<thread> threads;
    vector<int> sectionStarts(numThreads);
    int sectionSize = fileSize / numThreads;

    // Determine the start positions for each section
    for (int i = 0; i < numThreads; ++i) {
        sectionStarts[i] = i * sectionSize;
    }

    // Create threads to process each section of the file
    for (int i = 0; i < numThreads; ++i) {
        int start = sectionStarts[i];
        int end = (i == numThreads - 1) ? fileSize : sectionStarts[i + 1] - 1;

        threads.emplace_back(processSection, filename, start, end);
    }

    // Join all threads to wait for them to finish
    for (auto& thread : threads) {
        thread.join();
    }

    // Print the unique numbers from the global list
    cout << "Unique numbers in the file:" << endl;
    for (const auto& num : uniqueNumbers) {
        cout << num << endl;
    }

    return 0;
}
