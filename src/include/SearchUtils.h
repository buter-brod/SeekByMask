#ifndef SEARCHUTILS_H
#define SEARCHUTILS_H

#include <string>

struct PartInfo {

    PartInfo(const size_t start, const size_t end)
        : _start(start), _end(end) {

        _size = _end - _start + 1;
    }

    size_t _start{ 0 };
    size_t _end{ 0 };
    size_t _size{ 0 };
};

struct OccurrenceInfo {

    OccurrenceInfo(const size_t line, const size_t pos, const std::string& str) : _line(line), _pos(pos), _str(str) {}

    size_t _line{ 0 };
    size_t _pos{ 0 };
    std::string _str;
};

inline void assert(const bool what, const std::string& str = "") {

    if (!what) {
        throw std::exception(str.c_str());
    }
}

#endif