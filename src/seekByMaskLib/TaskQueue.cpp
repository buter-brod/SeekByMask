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
}

void TaskQueue::AddTask(const TaskPtr& taskPtr) {

	std::lock_guard<std::mutex> qLock(_queueMutex);
	_queue.push(taskPtr);
}

void TaskQueue::operator()() {

	do {
		const bool processed = process();

		if (!processed) {
			if (_endRequest) {
				break;
			}

			std::this_thread::yield();
		}
	}
	while (true);
}

bool TaskQueue::process() {

	TaskPtr currTask;

	{
		std::lock_guard<std::mutex> qLock(_queueMutex);
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
