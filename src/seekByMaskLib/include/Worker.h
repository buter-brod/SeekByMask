#ifndef WORKER_H
#define WORKER_H

#include "SearchUtils.h"

#include <memory>
#include <thread>
#include <deque>
#include <set>

class Worker {

public:

    typedef std::shared_ptr<Worker> Ptr;

    explicit Worker(const std::string& filename, const PartInfo& bounds, const std::string& mask);

    void Start();
    void Wait();

    const std::string& GetError() const {return _error;}

    const std::deque<OccurrenceInfo>& GetOccurrences() const { return _occurrences; }
    size_t GetLinesCount() const { return _linesCount; }

    static bool checkMask(const std::string& what, const size_t offset, const std::string& mask);
    static bool findByMask(const std::string& where, const std::string& mask, std::set<size_t>& results);

    void operator()();

protected:
    void process();

private:

    size_t _linesCount{ 0 };
    PartInfo _bounds;
    std::thread _thread;
    std::string _mask;
    std::string _filename;

    std::string _error;

    std::deque<OccurrenceInfo> _occurrences;
};

#endif