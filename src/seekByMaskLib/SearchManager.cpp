#include "SearchManager.h"
#include "Worker.h"
#include "FileWrapper.h"

#include <algorithm>

#if _HAS_CXX17
#include <filesystem>
#endif

size_t getFileSize(const std::string& filename) {

#if _HAS_CXX17
	// better
	const long sz = std::filesystem::file_size(filename);
	return sz;
#else
	FileHandle file(filename);
	const auto sz = file.GetSize();
	return sz;
#endif
}

void SearchManager::getPartBounds() {

	_parts.clear();

	const size_t fileSize = getFileSize(_filename);
	const size_t approxPartSize = std::max(_mask.size(), fileSize / _maxThreadsCount);

	FileHandle file(_filename);

	size_t currentPart = 0;
	size_t position = 0;

	while (position < fileSize) {

		const size_t assumedFirstByteOfNextPart = position + approxPartSize;
		const size_t assumedEndOfThisPartPos = assumedFirstByteOfNextPart - 1;

		file.Seek(assumedFirstByteOfNextPart);

		std::string line;
		file.ReadLine(line);

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

		_parts.insert({currentPart, {position, endPos}});

		position = endPos + 1;
		++currentPart;
	}

#ifndef NDEBUG
	if (_debugParts) {
		const std::string& outFileName = "partsDebugOutput.txt";
		dumpOutputParts(outFileName);
	}
#endif
}

void SearchManager::startWorkers() {

#ifdef TASK_QUEUE
	_taskQueue.Start();
#endif

	for (size_t partInd = 0; partInd < _parts.size(); ++partInd) {
		const PartInfo& part = _parts.at(partInd);

		Worker::Ptr workerPtr = std::make_shared<Worker>(_filename, part, _mask, &_resourceGuard);

		workerPtr->SetTaskQueue(&_taskQueue);

		_workers.insert({partInd, workerPtr});

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

void SearchManager::checkErrors() {

	for (auto& worker : _workers) {
		const std::string& err = worker.second->GetError();
		assert(err.empty(), "error in worker: " + err);
	}
}

void SearchManager::Start() {

	getPartBounds();
	startWorkers();
	waitWorkers();
	checkErrors();
	mergeWorkers();
}

#ifndef NDEBUG
#include <fstream>

void SearchManager::dumpOutputParts(const std::string& outputFilename) const {

	FileHandle file(_filename);
	std::string output;

	for (size_t i = 0; i < _parts.size(); i++) {
		const auto sz = _parts.at(i)._size;

		const std::string& partStr = file.Read(_parts.at(i)._start, sz);
		output += "[" + std::to_string(i) + "]" + partStr;
	}

	std::ofstream out(outputFilename, std::ios::binary);
	out << output;
	out.close();
}

#endif
