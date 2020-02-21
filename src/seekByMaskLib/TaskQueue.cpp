#include "TaskQueue.h"

void TaskQueue::Task::operator()() {
	if (_func) {
		try {
			_func();
		}
		catch (const std::exception& ex) {
			_error = ex.what();
		}
	}
}

TaskQueue::~TaskQueue() {

	Stop();

	if (_thread.joinable()) {
		_thread.join();
	}
}

void TaskQueue::Start() {
	_thread = std::thread(std::ref(*this));
}

void TaskQueue::Stop() {

	_endRequest = true;
	_qCondition.notify_one();
}

void TaskQueue::AddTask(const TaskPtr& taskPtr) {

	{
		std::lock_guard<std::mutex> qLock(_qMutex);
		_queue.push(taskPtr);
	}

	_qCondition.notify_one();
}

void TaskQueue::operator()() {

	bool processed = false;

	while (!_endRequest || processed) {

		processed = process();

		if (!processed) {
			std::unique_lock<std::mutex> qLock(_qMutex);
			const auto canResume = [this]() { return _endRequest || !_queue.empty(); };
			_qCondition.wait(qLock, canResume);
		}
	}
}

bool TaskQueue::process() {

	TaskPtr currTask;

	{
		std::lock_guard<std::mutex> qLock(_qMutex);
		if (!_queue.empty()) {
			currTask = _queue.front();
			_queue.pop();
		}
	}

	if (currTask) {
		(*currTask)();
		return true;
	}

	return false;
}
