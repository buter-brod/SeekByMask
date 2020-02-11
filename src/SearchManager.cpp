#include "SearchManager.h"
#include "Worker.h"

#include <fstream>
#include <algorithm>

#if _HAS_CXX17
#include <filesystem>
#endif

size_t getFileSize(const std::string& filename) {

#if _HAS_CXX17
    // reliable
    const long sz = std::filesystem::file_size(filename);
    return sz;
#else
    // some sources say that this method is not guaranteed to provide exact size in bytes
	std::ifstream file;
    file.open(filename, std::ios::in);
    assert(file.is_open(), "Unable to open input file");

    file.seekg(0, std::ios::end);
    const auto size = (size_t)file.tellg();
    file.close();

    return size;
#endif
}

void SearchManager::getPartBounds() {

    _parts.clear();

    const size_t fileSize = getFileSize(_filename);
    const size_t approxPartSize = std::max(_mask.size(), fileSize / _maxThreadsCount);

    FileHandle file(_filename);

    unsigned int currentPart = 0;
    size_t position = 0;

    while (position < fileSize) {

        const size_t assumedFirstByteOfNextPart = position + approxPartSize;
        const size_t assumedEndOfThisPartPos = assumedFirstByteOfNextPart - 1;

        file.seek(assumedFirstByteOfNextPart);

        const std::string& line = file.readLine();

        const bool sizeExceed = assumedEndOfThisPartPos >= fileSize - 1;
        const bool isLastPossiblePart = (currentPart == _maxThreadsCount - 1);

        if (sizeExceed && !isLastPossiblePart) {
	        /* case when actual parts/threads will be less than specified by _threadsCount.
	         * it depends on two factors:
	         * 1. average line length: if lines are relatively long, each part will exceed approxPartSize more aggresively
	         * 2. given desirable number of parts/threads: the more parts, the more likely last one (or even last few) will be empty.
	         */
        }

        size_t endPos = fileSize - 1;

        if (!sizeExceed) {
            const auto extraSize = line.size();
            endPos = std::min(endPos, assumedEndOfThisPartPos + extraSize);
        }

        _parts.insert({ currentPart, { position, endPos } });

        position = endPos + 1;
        ++currentPart;
    }

#if DEBUG_PARTS
    dumpOutputParts();
#endif
}

void SearchManager::startWorkers() {

    for (size_t partInd = 0; partInd < _parts.size(); ++partInd) {
        const PartInfo& part = _parts.at(partInd);

        Worker::Ptr workerPtr = std::make_shared<Worker>(_filename, part, _mask);
        _workers.insert({ partInd, workerPtr });

        workerPtr->Start();

        if (!_parallel) {
	        workerPtr->Wait();
        }
    }
}

void SearchManager::waitWorkers() {

    for (auto& worker : _workers) {
        worker.second->Wait();
    }
}

void SearchManager::mergeWorkers() {
	
    size_t linesCount = 0;

    for (auto& worker : _workers) {
        
        const auto& occurrences = worker.second->GetOccurrences();

        for (const auto& oi : occurrences) {
            const auto realLine = oi._line + linesCount + (countStartFrom1 ? 1 : 0);
            _results.emplace_back(realLine, oi._pos + (countStartFrom1 ? 1 : 0), oi._str);
        }

        const auto workerLines = worker.second->GetLinesCount();
        linesCount += workerLines;
    }
}

void SearchManager::Start() {

    getPartBounds();
    startWorkers();
    waitWorkers();
    mergeWorkers();	
}

#if DEBUG_PARTS

void SearchManager::dumpOutputParts(const std::string& outputFilename) const{
    
    FileHandle file(_filename);
    std::string output;

    for (size_t i = 0; i < _parts.size(); i++) {
        const auto sz = _parts.at(i)._size;

        const std::string& partStr = file.read(_parts.at(i)._start, sz);
        output += "[" + std::to_string(i) + "]" + partStr;
    }

    std::ofstream out(outputFilename, std::ios::binary);
    out << output;
    out.close();
}

#endif