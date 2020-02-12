#include "SearchManager.h"
#include "FileWrapper.h"

#include <chrono>
#include <iostream>

const std::string defaultMask = "c?ap";
static const std::string defaultFilename = "../Resources/oxford_dict.txt";

size_t getCoresNum() {
    static auto coresNum = (size_t)std::thread::hardware_concurrency();
    return coresNum;
}

long long getTimeMCS() {
    const auto now = std::chrono::system_clock::now();
    const auto duration = now.time_since_epoch();
    const auto mcs = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();

    return mcs;
}

float testSearch(const std::string& filename, const std::string& mask, const size_t threadsNum) {

    const auto time0 = getTimeMCS();
    SearchManager sm(filename, mask, threadsNum);
    sm.Start();
    const auto time1 = getTimeMCS();

    const float dtSec = float(time1 - time0) / 1000 / 1000;

    return dtSec;
}

size_t maxLineLenCount(const std::string& filename) {

    std::ifstream file;
    file.open(filename, std::ios::in);
    assert(file.is_open(), "unable to open file " + filename);

    size_t maxLen = 0;
    std::string line;

    do {
        getline(file, line);

        if (line.size() > maxLen) {
            maxLen = line.size();
        }
    } while (!file.eof());

    file.close();
    return maxLen;
}

size_t testRun(const std::string& filename, const std::string& mask) {

    const auto longestLineSize = maxLineLenCount(filename) + 2; // respect LF or CRLF endings

    assert(longestLineSize < FileHandle::strBufferLen, std::string("input file has very long lines, l=") + std::to_string(longestLineSize) + ", max=" + std::to_string(FileHandle::strBufferLen));

    const auto cores = getCoresNum();
    const size_t maxThreads = 1024;

    float bestTime = 100500.f;
    size_t bestThreads = 0;

    std::cout << "TESTING PERFORMANCE!\n";
    std::cout << "HARDWARE CORES=" << cores << ", do you expect threads=" << cores << " to give best results?\n";

    size_t threads = 1;
    while (threads <= maxThreads) {

        const float time = testSearch(filename, mask, threads);
        std::cout << "threads: " << threads << ", time: " << time << "\n";

        if (time < bestTime) {
            bestThreads = threads;
            bestTime = time;
        }

        if (threads < 10) {
            threads++;
        }
        else if (threads == 10) {
            threads = 16;
        }
        else {
            threads = threads * 2;
        }
    }

    std::cout << "-------------------------------\n";
    std::cout << "best result: threads=" << bestThreads << ", time=" << bestTime << "\n";

    return bestThreads;
}

int main() {

    testRun(defaultFilename, defaultMask);

  return 0;
}