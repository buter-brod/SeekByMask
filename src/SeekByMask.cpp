#include "include/SearchManager.h"

#include <iostream>
#include <chrono>

const std::string defaultMask = "c?ap";
static const std::string defaultFilename = "../Resources/oxford_dict_large.txt";

static const bool needTestsDefault = false;

long long getTimeMCS() {
    const auto now = std::chrono::system_clock::now();
    const auto duration = now.time_since_epoch();
    const auto mcs = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();

    return mcs;
}

size_t getCoresNum() {
    static auto coresNum = (size_t) std::thread::hardware_concurrency();
	return coresNum;
}

float testSearch(const std::string& filename, const std::string& mask, const size_t threadsNum) {
	
    const auto time0 = getTimeMCS();
    SearchManager sm(filename, mask, threadsNum);
    sm.Start();
    const auto time1 = getTimeMCS();

    const float dtSec = float(time1 - time0) / 1000 / 1000;

    return dtSec;
}

size_t testRun(const std::string& filename, const std::string& mask) {
	
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
        } else if (threads == 10) {
	        threads = 16;
        } else {
	        threads = threads * 2;
        }
    }

    std::cout << "-------------------------------\n";
    std::cout << "best result: threads=" << bestThreads << ", time=" << bestTime << "\n";

    return bestThreads;
}

void normalRun(const std::string& filename, const std::string& mask, const size_t threadsNum) {
    
    SearchManager sm(filename, mask, threadsNum);
    sm.Start();
    const auto& results = sm.GetResults();

    std::cout << results.size() << "\n";

    for (auto& occInfo : results) {
        std::cout << occInfo._line << " " << occInfo._pos << " " << occInfo._str << "\n";
    }
}

int main(int argc, char* argv[]) {

    std::string filename = defaultFilename;
    std::string mask = defaultMask;
    bool needTests = needTestsDefault;

    if (argc > 1) {
	    filename = argv[1];
    }

    if (argc > 2) {
        mask = argv[2];
    }

    if (argc > 3) {

        const bool testsParamSet = std::string(argv[3]) == "tests";

        if (!testsParamSet) {
            std::cout << "error: third param may only be \"tests\"\n";
            return 0;
        }

    	needTests = true;
    }   

    try {
        if (needTests) {
            const size_t bestThreads = testRun(filename, mask);
            normalRun(filename, mask, bestThreads);
        }
        else {
            const auto coresNum = getCoresNum();
            normalRun(filename, mask, coresNum);
        }
    }
    catch (const std::exception& ex) {
	    std::cout << "error! " << ex.what() << "\n";
    }
}

