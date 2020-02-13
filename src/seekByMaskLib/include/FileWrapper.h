#ifndef FILEWRAPPER_H
#define FILEWRAPPER_H

#include "SearchUtils.h"

class FileHandle {

public:

	explicit FileHandle(const std::string& fn);
	~FileHandle();

	size_t GetCursor() const;
	size_t GetSize();

	void Seek(const size_t where);
	std::string Read(const size_t where, const size_t size);
	void ReadLine(std::string& output);

protected:
	void open();
	bool isOpen() const;
	void close();

private:
	std::string _filename;

	size_t _cursor{ 0 };
	FILE* _file{ nullptr };
};

#endif