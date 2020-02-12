#include "Worker.h"
#include "FileWrapper.h"

Worker::Worker(const std::string& filename, const PartInfo& bounds, const std::string& mask) : _bounds(bounds), _mask(mask), _filename(filename) {}

void Worker::Start() {
    _thread = std::thread(std::ref(*this));
}

void Worker::Wait() {
    if (_thread.joinable()) {
		_thread.join();
    }
}

bool Worker::checkMask(const std::string& what, const size_t offset, const std::string& mask) {

    const auto sz = mask.size();

    for (size_t i = 0; i < sz; ++i) {

        const bool symbolOk = (mask[i] == what[i + offset]) || (mask[i] == '?');
        if (!symbolOk) {
            return false;
        }
    }

    return true;
}

std::set<size_t> Worker::findByMask(const std::string& where, const std::string& mask) {

    const auto maskSize = (size_t)mask.size();

    std::set<size_t> results;

    if (where.size() < maskSize) {
        return results;
    }

    size_t i = 0;
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

void Worker::process() {

    FileHandle file(_filename);
    file.Seek(_bounds._start);

    bool outOfBounds = false;

    size_t position = _bounds._start;

    do {

        const std::string& line = file.ReadLine();
        position += line.size();
        const auto& where = findByMask(line, _mask);

        for (const auto& foundPos : where) {
            const std::string str = line.substr(foundPos, _mask.size());
            _occurrences.emplace_back(_linesCount, foundPos, str);
        }

        outOfBounds = position > _bounds._end;
        ++_linesCount;

    } while (!outOfBounds);
}

void Worker::operator()() {

    try {
	    process();
    }
    catch(const std::exception& ex) {
	    _error = ex.what();
    }
}