#include "SearchManager.h"

#include <chrono>
#include <iostream>

const std::string threadsTestMask = "c?ap";
static const std::string threadsTestFilename = "../Resources/oxford_dict.txt";

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

size_t numThreadsTestRun(const std::string& filename, const std::string& mask) {

	const auto cores = (size_t)std::thread::hardware_concurrency();
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

bool casesRun() {

	{
		SearchManager sm("../Resources/win_endings.txt", "mask", 4);
		sm.Start();
		const auto results = sm.GetResults();

		const std::string& winEndingsFailMsg = "win_endings test failed";

		assert(results.size() == 2, winEndingsFailMsg);
		assert(results[0]._line == 5 && results[0]._pos == 1, winEndingsFailMsg);
		assert(results[1]._line == 10 && results[1]._pos == 1, winEndingsFailMsg);
	}

	{
		SearchManager sm("../Resources/unix_endings.txt", "mask", 8);
		sm.Start();
		const auto results = sm.GetResults();

		const std::string& unixEndingsFailMsg = "unix_endings test failed";

		assert(results.size() == 2, unixEndingsFailMsg);
		assert(results[0]._line == 3 && results[0]._pos == 2, unixEndingsFailMsg);
		assert(results[1]._line == 6 && results[1]._pos == 2, unixEndingsFailMsg);
	}

	{
		SearchManager sm("../Resources/nooverlap.txt", "????", 4);
		sm.Start();
		const auto results = sm.GetResults();

		const std::string nooverlapFailMsg = "nooverlap test failed";

		assert(results.size() == 2, nooverlapFailMsg);
		assert(results[0]._line == 1 && results[0]._pos == 1, nooverlapFailMsg);
		assert(results[1]._line == 1 && results[1]._pos == 5, nooverlapFailMsg);
	}

	{
		SearchManager sm("../Resources/oxford_dict.txt", "c?ap", 64);
		sm.Start();
		const auto results = sm.GetResults();

		const std::string oxfordFailMsg = "oxford test failed";

		assert(results.size() == 122, oxfordFailMsg);
		assert(results.rbegin()->_line == 72238 && results.rbegin()->_pos == 129, oxfordFailMsg);
	}

	std::cout << "all cases passed well!\n";

	return true;
}

int main() {

	try {
		casesRun();
		numThreadsTestRun(threadsTestFilename, threadsTestMask);
	}
	catch (const std::exception & ex) {
		std::cout << "TESTS FAILED! Error: " << ex.what() << "\n";
	}

	return 0;
}