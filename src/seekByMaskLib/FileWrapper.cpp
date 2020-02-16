#include "FileWrapper.h"

static constexpr size_t strBufLenDefault = 128;

FileHandle::FileHandle(const std::string& fn) : _filename(fn) {
	open();
}

FileHandle::~FileHandle() {
	if (isOpen()) {
		close();
	}
}

size_t FileHandle::GetCursor() const {
	return _cursor;
}

size_t FileHandle::GetSize() {

	assert(isOpen(), "FileHandle::Seek file isn't open");

	fseek(_file, 0L, SEEK_END);
	const size_t size = ftell(_file);

	Seek(_cursor);

	return size;
}

void FileHandle::open() {
	const auto err = fopen_s(&_file, _filename.c_str(), "rb");

	if (err != 0) {
		_file = nullptr;
		assert(false, "cannot open file, err=" + std::to_string(err));
	}
}

bool FileHandle::isOpen() const {
	return _file;
}

void FileHandle::close() {
	const auto err = fclose(_file);
	_file = nullptr;
	// todo check err and report somehow without throwing?
}

void FileHandle::Seek(const size_t where) {
	assert(isOpen(), "FileHandle::Seek file isn't open");
	_cursor = where;
	const auto err = fseek(_file, (long)where, SEEK_SET);
	assert(err == 0, "file seek error");
}

std::string FileHandle::Read(const size_t where, const size_t size) {

	assert(isOpen(), "FileHandle::Seek file isn't open");
	Seek(where);

	std::string str;
	str.resize(size, 0);

	void* strDataPtr = (void*)str.data();

	fread_s(strDataPtr, size, size, 1, _file);

	_cursor += str.size();

	return str;
}

void FileHandle::ReadLine(std::string& output) {

	assert(isOpen(), "FileHandle::Seek file isn't open");

	output.clear();

	size_t bufSize = strBufLenDefault;
	bool exceed = false;

	do {
		size_t writtenBytes = 0;
		size_t toRead = bufSize;

		if (!output.empty()) {
			writtenBytes = bufSize - 1;
			toRead = bufSize * 2 + 1;
			bufSize *= 3;
		}

		output.resize(bufSize, 0);

		char* strDataPtr = (char*)output.data() + writtenBytes;
		fgets(strDataPtr, (int)toRead, _file);

		const char lastWrittenSym = output[bufSize - 2];
		exceed = lastWrittenSym == 0 || lastWrittenSym == '\n';
	}
	while (!exceed);

	const char nullterm = 0;
	output.resize(output.find(nullterm));

	_cursor += output.size();
}
