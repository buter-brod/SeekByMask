#include <iostream>
#include <fstream>

#include <string>
#include <map>
#include <set>
#include <thread>
#include <deque>

#include <memory>

//#define DEBUG_PARTS 1

#if _HAS_CXX17
#include <filesystem>
#endif

static const std::string inputFilename = "Resources/oxford_dict.txt";
static const unsigned int threadsCount = 8;//std::thread::hardware_concurrency(); //todo: remove from staticc1!111

unsigned long getFileSize(const std::string& filename) {

#if _HAS_CXX17
    // reliable
    const long sz = std::filesystem::file_size(filename);
    return sz;
#else
    // some sources say that this method is not guaranteed to provide exact size in bytes
    std::ifstream file;
    file.open(filename, std::ios::in);
    file.seekg(0, std::ios::end);
    const long size = (long)file.tellg();
    file.close();

    return size;
#endif
}

void assert(const bool what, const std::string& str) {
	
    if (!what) {
        throw std::exception(str.c_str());	    
    }
}

struct PartInfo {

    PartInfo(unsigned int ind, unsigned long start, unsigned long end)
        :_ind(ind), _start(start), _end(end) {

    	_size = _end - _start + 1;
    }

	int _ind{0};
    unsigned long _start {0};
    unsigned long _end   {0};
    unsigned long _size  {0};
};

struct OccurrenceInfo {

    OccurrenceInfo(const unsigned long line, const unsigned long pos, const std::string str) : _line(line), _pos(pos), _str(str) {}

    std::string _str;
    unsigned long _line{0};
    unsigned long _pos{0};
};


bool checkMask(const std::string& what, const unsigned long offset, const std::string& mask) {

    const auto sz = mask.size();

    for (size_t i = 0; i < sz; ++i) {

        const bool symbolOk = (mask[i] == what[i + offset]) || (mask[i] == '?');
        if (!symbolOk) {
            return false;
        }
    }

    return true;
}

std::set<unsigned long> findByMask(const std::string& where, const std::string& mask) {

    const auto maskSize = (unsigned long)mask.size();

    std::set<unsigned long> results;
    
	if (where.size() < maskSize) {
		return results;
	}    

    unsigned long i = 0;
    while (i <= where.size() - maskSize) {
        const bool foundHere = checkMask(where, i, mask);

        if (foundHere) {
            results.insert(i);
            i += maskSize;
        }
        else {
            ++i;
        }
    }

    return results;
}

class WorkerInfo {

public:

    typedef std::shared_ptr<WorkerInfo> Ptr;

    explicit WorkerInfo(const PartInfo& bounds, const std::string& mask) : _bounds(bounds), _mask(mask) {}

    void Start() {
    	_thread = std::thread(std::ref(*this));
    }

    void Wait() {
	    _thread.join();
    }

    const std::deque<OccurrenceInfo>& GetOccurrences() const {return _occurrences;}

    unsigned long GetLinesCount() const {return _totalLines;}

    void operator()() {
    	
        std::ifstream file;
        file.open(inputFilename, std::ios::in);
        file.seekg(_bounds._start, std::ios::beg);
        std::string line;

        bool outOfBounds = false;

        unsigned long lineInd = 0;

    	do {

            getline(file, line);

            const auto& where = findByMask(line, _mask);

            for (const auto& foundPos : where) {
                const std::string str = line.substr(foundPos, _mask.size());
	            _occurrences.emplace_back(lineInd, foundPos, str);
            }

            const auto pos = file.tellg();
            outOfBounds = pos >= _bounds._end;
            ++lineInd;
    	}
        while (!outOfBounds);

        _totalLines = lineInd;
    }

private:

    unsigned long _totalLines{ 0 };
    PartInfo _bounds;
    std::thread _thread;
    std::string _mask;

    std::deque<OccurrenceInfo> _occurrences;
};

std::string readFromFile(std::ifstream& stream, const unsigned long size) {

    char* buf = new char[size];
    stream.read(buf, size);
    const std::string str(buf, size);
    delete[] buf;

    return str;
}

#ifdef DEBUG_PARTS

void debugOutputParts(const std::map<unsigned int, PartInfo >& partBounds, const std::string& filename) {

    std::ifstream file;
    file.open(filename, std::ios::in);
    std::string output;
    const std::string& outputFilename = "output.txt";

    for (int i = 0; i < partBounds.size(); i++) {
        const auto sz = partBounds.at(i)._size;

        file.seekg(partBounds.at(i)._start, std::ios::beg);
        const std::string partStr = readFromFile(file, sz);

        output += "PART " + std::to_string(i) + ":\n" + partStr + "\n";
    }

    std::ofstream out(outputFilename);
    out << output;
    out.close();
}

#endif

std::map<unsigned int, PartInfo > getPartBounds() {
	
    const unsigned long fileSize = getFileSize(inputFilename);
    const unsigned long approxPartSize = fileSize / threadsCount;

    std::map<unsigned int, PartInfo > partBounds;

    std::string line;
    std::ifstream file;
    file.open(inputFilename, std::ios::in);

    unsigned int currentPart = 0;
    unsigned long position = 0;

    while (currentPart < threadsCount) {

        const unsigned long assumedEndOfThisPartPos = position + approxPartSize;
        file.seekg(assumedEndOfThisPartPos, std::ios::beg);
        getline(file, line);

        const bool sizeExceed = assumedEndOfThisPartPos >= fileSize;
        const bool isLastPart = currentPart == threadsCount - 1;
        assert(isLastPart == sizeExceed, "something wrong with last part?");

        unsigned long endPos = fileSize - 1;

        if (!isLastPart) {
            const auto extraSize = (unsigned long)line.size();
            const unsigned long realSize = approxPartSize + extraSize - 1;
            endPos = position + realSize;
        }

        partBounds.insert({ currentPart, { currentPart, position, endPos } });

        position = endPos + 2;

        auto symAtPos = [&file](const unsigned long pos) -> std::string {

            file.seekg(pos, std::ios::beg);
            const std::string& symStr = readFromFile(file, 1);
            return symStr;
        };

        ++currentPart;
    }

    return partBounds;
}

int main() {
    const auto& parts = getPartBounds();

#ifdef DEBUG_PARTS
    debugOutputParts(parts, inputFilename);
#endif

    const std::string mask = "c?ap";

    std::map<int, WorkerInfo::Ptr> workers;

    for (int partInd = 0; partInd < parts.size(); ++partInd) {
	    const PartInfo& pi = parts.at(partInd);

        WorkerInfo::Ptr workerPtr = std::make_shared<WorkerInfo>(pi, mask);
        workers.insert({partInd, workerPtr});

        workerPtr->Start();
    }

    for (auto& worker : workers) {
	    worker.second->Wait();
    }

    unsigned long linesCount = 0;

    std::string result;

    std::deque<OccurrenceInfo> finalInfo;

    for (auto& worker : workers) {
        const auto workerLines = worker.second->GetLinesCount();

        const auto& occurrences = worker.second->GetOccurrences();

        for (const auto& oi : occurrences) {
            const auto realLine = oi._line + linesCount + 1;
            finalInfo.emplace_back(realLine, oi._pos + 1, oi._str);
        }

    	linesCount += workerLines;
    }

    std::cout << "RESULTS:\n";

    std::cout << finalInfo.size() << "\n";

    for (auto& occInfo : finalInfo) {
	    std::cout << occInfo._line<< " " << occInfo._pos << " " << occInfo._str << "\n";
    }

    std::cout << "Done!\n";
    //todo: show time
}

