#include <iostream>
#include <unordered_map>
#include <list>
#include <string>
#include <ctime>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Struct to store file information
struct FileInfo {
    string filePath;
    time_t openTimestamp;
    size_t fileSize;
    time_t fileTimestamp;
    int fileInode;
};

class LRUCache {
private:
    int capacity;
    unordered_map<string, list<pair<string, FileInfo>>::iterator> cacheMap;
    list<pair<string, FileInfo>> cacheList;
    seconds maxAge; // Maximum age for a file in seconds

public:
    LRUCache(int capacity, int maxAgeSeconds) : capacity(capacity), maxAge(maxAgeSeconds) {}

    void addFile(const string& filePath, time_t openTimestamp, size_t fileSize, time_t fileTimestamp, int fileInode) {
        // Create file info object
        FileInfo fileData = {filePath, openTimestamp, fileSize, fileTimestamp, fileInode};

        // Check if the file is already in the cache
        if (cacheMap.find(filePath) != cacheMap.end()) {
            // Update file information
            auto it = cacheMap[filePath];
            it->second = fileData;
            // Move the file to the front (as it's the most recently used)
            cacheList.splice(cacheList.begin(), cacheList, it);
        } else {
            // Add the file to the cache
            if (cacheList.size() >= capacity) {
                // Remove the least recently used file (at the end)
                auto last = cacheList.back();
                cacheMap.erase(last.first);
                cacheList.pop_back();
            }
            // Add the new file to the front
            cacheList.emplace_front(filePath, fileData);
            cacheMap[filePath] = cacheList.begin();
        }
    }

    FileInfo getFile(const string& filePath) {
        if (cacheMap.find(filePath) != cacheMap.end()) {
            // Get the file information and move it to the front (as it's the most recently used)
            auto it = cacheMap[filePath];
            cacheList.splice(cacheList.begin(), cacheList, it);
            return it->second;
        }
        // Return empty file information if not found
        return {"", 0, 0, 0, 0};
    }

    void removeOldEntries() {
        auto currentTime = time(nullptr);
        for (auto it = cacheList.begin(); it != cacheList.end();) {
            if (currentTime - it->second.openTimestamp > maxAge.count()) {
                cacheMap.erase(it->first);
                it = cacheList.erase(it);
            } else {
                ++it;
            }
        }
    }
};

// Example usage
int main() {
    LRUCache cache(5, 60); // Capacity of 5 and maximum age of 60 seconds

    // Adding file information to the cache
    cache.addFile("xyz.txt", time(nullptr) - 30, 1024, time(nullptr), 12345);
    cache.addFile("abc.txt", time(nullptr) - 70, 2048, time(nullptr), 67890);

    // Retrieve file information from the cache
    FileInfo file1 = cache.getFile("file1.txt");
    cout << "File 1 size: " << file1.fileSize << endl;
    FileInfo file2 = cache.getFile("file2.txt");
    cout << "File 2 size: " << file2.fileSize << endl;

    // Clean the cache of old entries
    cache.removeOldEntries();

    return 0;
}
