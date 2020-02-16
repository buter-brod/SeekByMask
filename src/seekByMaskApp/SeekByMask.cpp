#include "SearchManager.h"
#include <iostream>

const std::string defaultMask = "c?ap";
static const std::string defaultFilename = "../Resources/oxford_dict.txt";

void run(const std::string& filename, const std::string& mask, const size_t threadsNum) {

	SearchManager sm(filename, mask, threadsNum);
	//sm.SetDebugParts();
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

	if (argc > 1) {
		filename = argv[1];
	}

	if (argc > 2) {
		mask = argv[2];
	}

	try {
		const auto coresNum = std::thread::hardware_concurrency();
		run(filename, mask, coresNum);
	}
	catch (const std::exception& ex) {
		std::cout << "error: " << ex.what() << "\n";
	}
}
