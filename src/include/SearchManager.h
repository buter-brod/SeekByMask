#ifndef SEARCHMANAGER_H
#define SEARCHMANAGER_H
#include "SearchUtils.h"
#include <map>
#include "Worker.h"

#define DEBUG_PARTS 1

class SearchManager {

	public:

	SearchManager(const std::string& filename, const std::string& mask, const size_t threads) : _filename(filename), _mask(mask), _threadsCount(threads) {}

	void Start();

	const std::deque<OccurrenceInfo>& GetResults() const {return _results;}

protected:
    void getPartBounds();
	void startWorkers();
	void waitWorkers();
	void mergeWorkers();

#ifdef DEBUG_PARTS
    void debugOutputParts(const std::string& outputFilename = "partsDebugOutput.txt") const;
#endif

private:

	// input data
	std::string _filename;
	std::string _mask;
	size_t _threadsCount{1};
	bool countStartFrom1{true};

	// working data
	std::map<size_t, PartInfo > _parts;
	std::map<size_t, Worker::Ptr> _workers;

	//output data
	std::deque<OccurrenceInfo> _results;
};


#endif