#ifndef RESOURCE_PROVIDER_H
#define RESOURCE_PROVIDER_H

#include <thread>
#include <functional>
#include <queue>
#include <string>
#include <mutex>
#include <atomic>

class TaskQueue {

public:

	struct Task {

		explicit Task(const std::function<void()>& func) : _func(func) {}
		void operator()();

		std::function<void()> _func;
		std::string _error;
	};

	using TaskPtr = std::shared_ptr<Task>;
	using Queue = std::queue<TaskPtr>;

	struct ErrReport {
		ErrReport(const TaskPtr& task, const std::string& errStr) : _task(task), _errStr(errStr) {}
		TaskPtr _task;
		std::string _errStr;
	};

	using ErrReports = std::deque<ErrReport>;

	TaskQueue() = default;
	~TaskQueue();

	void operator()();

	void AddTask(const TaskPtr& taskPtr);
	void Start();
	void Stop();

protected:
	bool process();
	void error(const TaskPtr& task, const std::string& errStr);

private:
	std::thread _thread;
	Queue _queue;

	std::atomic<bool> _endRequest{false};

	std::mutex _queueMutex;
};

#endif
