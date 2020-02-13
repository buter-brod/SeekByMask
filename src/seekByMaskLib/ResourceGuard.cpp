#include "ResourceGuard.h"

ResourceLock::ResourceLock(std::mutex& m) {

	_lockPtr = std::make_shared<LockGuard>(m);
}

ResourceLock::Ptr ResourceGuard::GetLock(const std::string& resName) {

	ResourceLock::Ptr lockPtr;

#ifdef LOCK_READ
	std::lock_guard<std::mutex> lockRG(_ownMutex);

	std::mutex& mutex = _resources[resName];
	lockPtr = std::make_shared<ResourceLock>(mutex);

#endif
	return lockPtr;
}