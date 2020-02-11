#include "SearchManager.h"
#include "Worker.h"

#include <fstream>

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
    const size_t approxPartSize = fileSize / _threadsCount;

    std::string line;
    std::ifstream file;
    file.open(_filename, std::ios::in);
    assert(file.is_open(), "Unable to open input file");

    unsigned int currentPart = 0;
    size_t position = 0;

    while (currentPart < _threadsCount) {

        const size_t assumedEndOfThisPartPos = position + approxPartSize;
        file.seekg(assumedEndOfThisPartPos, std::ios::beg);
        getline(file, line);

        const bool sizeExceed = assumedEndOfThisPartPos >= fileSize;
        const bool isLastPart = currentPart == _threadsCount - 1;
        assert(isLastPart == sizeExceed, "something wrong with last part?");

        size_t endPos = fileSize - 1;

        if (!isLastPart) {
            const auto extraSize = (size_t)line.size();
            const size_t realSize = approxPartSize + extraSize - 1;
            endPos = position + realSize;
        }

        _parts.insert({ currentPart, { position, endPos } });

        position = endPos + 2;

        ++currentPart;
    }
}

void SearchManager::startWorkers() {
	
    for (size_t partInd = 0; partInd < _parts.size(); ++partInd) {
        const PartInfo& part = _parts.at(partInd);

        Worker::Ptr workerPtr = std::make_shared<Worker>(_filename, part, _mask);
        _workers.insert({ partInd, workerPtr });

        workerPtr->Start();
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

#ifdef DEBUG_PARTS
    debugOutputParts();
#endif

    startWorkers();
    waitWorkers();
    mergeWorkers();	
}

#ifdef DEBUG_PARTS

void SearchManager::debugOutputParts(const std::string& outputFilename) const{
    
    std::ifstream file;
    file.open(_filename, std::ios::in);
    assert(file.is_open(), "Unable to open input file");
    std::string output;

    const auto read = [&file](const size_t size) -> std::string {
        char* buf = new char[size];
        file.read(buf, size);
        const std::string str(buf, size);
        delete[] buf;

        return str;
    };

    for (size_t i = 0; i < _parts.size(); i++) {
        const auto sz = _parts.at(i)._size;

        file.seekg(_parts.at(i)._start, std::ios::beg);
        const std::string partStr = read(sz);

        output += "PART " + std::to_string(i) + ":\n" + partStr + "\n";
    }

    std::ofstream out(outputFilename);
    out << output;
    out.close();
}

#endif