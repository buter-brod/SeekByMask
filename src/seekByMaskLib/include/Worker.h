#ifndef WORKER_H
#define WORKER_H


#include <memory>
#include <thread>
#include <deque>
#include <set>
#include "ResourceGuard.h"

class ResourceGuard;

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

class Worker {

public:

	typedef std::shared_ptr<Worker> Ptr;

	explicit Worker(const std::string& filename, const PartInfo& bounds, const std::string& mask, ResourceGuard* rg = nullptr);

	void Start();
	void Wait();

	const std::string& GetError() const { return _error; }

	const std::deque<OccurrenceInfo>& GetOccurrences() const { return _occurrences; }
	size_t GetLinesCount() const { return _linesCount; }

	static bool checkMask(const std::string& what, const size_t offset, const std::string& mask);
	static bool findByMask(const std::string& where, const std::string& mask, std::set<size_t>& results);

	void operator()();

protected:
	void process();
	ResourceLock::Ptr tryLock() const;

private:

	size_t _linesCount{ 0 };
	PartInfo _bounds;
	std::thread _thread;
	std::string _mask;
	std::string _filename;

	std::string _error;

	ResourceGuard* _resourceGuard{ nullptr };

	std::deque<OccurrenceInfo> _occurrences;
};

#endif