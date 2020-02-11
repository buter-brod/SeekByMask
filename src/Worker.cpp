#include "Worker.h"

Worker::Worker(const std::string& filename, const PartInfo& bounds, const std::string& mask) : _bounds(bounds), _mask(mask), _filename(filename) {}

void Worker::Start() {
    _thread = std::thread(std::ref(*this));
}

void Worker::Wait() {
    _thread.join();
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

void Worker::operator()() {

    std::ifstream file;
    file.open(_filename, std::ios::in);
    assert(file.is_open(), "Unable to open input file");
    file.seekg(_bounds._start, std::ios::beg);
    std::string line;

    bool outOfBounds = false;

    do {

        getline(file, line);

        const auto& where = findByMask(line, _mask);

        for (const auto& foundPos : where) {
            const std::string str = line.substr(foundPos, _mask.size());
            _occurrences.emplace_back(_linesCount, foundPos, str);
        }

        const auto pos = file.tellg();
        outOfBounds = (size_t)pos >= _bounds._end;
        ++_linesCount;

    } while (!outOfBounds);

}