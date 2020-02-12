#ifndef SEARCHMANAGER_H
#define SEARCHMANAGER_H

//todo disable
//#define DEBUG_PARTS 1

#include "SearchUtils.h"
#include "Worker.h"
#include <map>

class SearchManager {

	public:

	SearchManager(const std::string& filename, const std::string& mask, const size_t threads) : _filename(filename), _mask(mask), _maxThreadsCount(threads) {}
	void Start();

#ifndef NDEBUG
	void SetDebugParts() {_debugParts = true;}
#endif

	const std::deque<OccurrenceInfo>& GetResults() const {return _results;}

protected:
    void getPartBounds();
	void startWorkers();
	void waitWorkers();
	void checkErrors();
	void mergeWorkers();

#ifndef RELEASE
    void dumpOutputParts(const std::string& outputFilename = "partsDebugOutput.txt") const;
#endif

private:
	// input data
	std::string _filename;
	std::string _mask;

	size_t _maxThreadsCount{1};

	// for results output: should lines and rows numeration start from 0 or 1
	bool countStartFrom1{true};

	// working data
	std::map<size_t, PartInfo > _parts;
	std::map<size_t, Worker::Ptr> _workers;

	bool _parallel{true};

#ifndef NDEBUG
	bool _debugParts{false};
#endif

	//output data
	std::deque<OccurrenceInfo> _results;
};


#endif