#include "Worker.h"
#include "FileWrapper.h"
#include "ResourceGuard.h"
#include "TaskQueue.h"

#include <future>

constexpr bool useTaskQueue =
#ifdef TASK_QUEUE
	true;
#else
	false;
#endif

Worker::Worker(const std::string& filename, const PartInfo& bounds, const std::string& mask, ResourceGuard* rg) :
	_bounds(bounds), _mask(mask), _filename(filename), _resourceGuard(rg) {}

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

		const char checkSymbol = what[i + offset];

		if (checkSymbol == '\r' || checkSymbol == '\n') {
			return false;
		}

		const char maskSymbol = mask[i];

		if (maskSymbol != '?' && maskSymbol != checkSymbol) {
			return false;
		}
	}

	return true;
}

bool Worker::findByMask(const std::string& where, const std::string& mask, std::set<size_t>& results) {

	const auto maskSize = (size_t)mask.size();

	if (where.size() < maskSize) {
		return false;
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

	return !results.empty();
}

ResourceLock::Ptr Worker::tryLock() const {

	ResourceLock::Ptr lockPtr;
	if (_resourceGuard) {
		lockPtr = _resourceGuard->GetLock(_filename);
	}

	return lockPtr;
}

void Worker::getLine(FileHandle* file, std::string& line) {

	std::promise<std::string> readLnPromise;
	const TaskQueue::TaskPtr getLineTaskPtr = std::make_shared<TaskQueue::Task>([this, &file, &readLnPromise]() {

		std::string line;
		{
			const auto& lock = tryLock();
			file->ReadLine(line);
		}
		readLnPromise.set_value(line);
	});

	if (useTaskQueue && _taskQueue) {
		_taskQueue->AddTask(getLineTaskPtr);
	}
	else {
		(*getLineTaskPtr)();
	}

	line = readLnPromise.get_future().get();

	if (!getLineTaskPtr->_error.empty()) {
		throw std::exception(getLineTaskPtr->_error.c_str());
	}
}

void Worker::process() {

	FileHandle file(_filename);
	file.Seek(_bounds._start);

	bool outOfBounds = false;

	size_t position = _bounds._start;

	do {
		std::string line;
		getLine(&file, line);

		position += line.size();
		std::set<size_t> places;
		const bool found = findByMask(line, _mask, places);

		if (found) {
			for (const auto& place : places) {
				const std::string& str = line.substr(place, _mask.size());
				_occurrences.emplace_back(_linesCount, place, str);
			}
		}

		outOfBounds = position > _bounds._end;
		++_linesCount;

	}
	while (!outOfBounds);
}

void Worker::operator()() {

	try {
		process();
	}
	catch (const std::exception& ex) {
		_error = ex.what();
	}
}
