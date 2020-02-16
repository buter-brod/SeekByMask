#ifndef RESOURCE_GUARD_H
#define RESOURCE_GUARD_H

#include <string>
#include <mutex>
#include <unordered_map>
#include <memory>

class ResourceLock {
public:
	typedef std::shared_ptr<ResourceLock> Ptr;
	explicit ResourceLock(std::mutex& m);

private:
	typedef std::lock_guard<std::mutex> LockGuard;
	std::shared_ptr<LockGuard> _lockPtr;
};

class ResourceGuard {
public:
	ResourceLock::Ptr GetLock(const std::string& resName);

private:
	std::unordered_map<std::string, std::mutex> _resources;
	std::mutex _ownMutex;
};

#endif
