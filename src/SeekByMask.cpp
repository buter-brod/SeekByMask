#include <iostream>
#include "include/SearchManager.h"

static const std::string inputFilename = "../Resources/oxford_dict.txt";
static const unsigned int threadsCount = 8;//std::thread::hardware_concurrency(); //todo: remove from staticc1!111

int main() {

    const std::string mask = "c?ap";

    SearchManager sm(inputFilename, mask, 4);
    sm.Start();
    const auto& results = sm.GetResults();

    std::cout << results.size() << "\n";

    for (auto& occInfo : results) {
	    std::cout << occInfo._line<< " " << occInfo._pos << " " << occInfo._str << "\n";
    }

    std::cout << "Done!\n";
    //todo: show time
}

