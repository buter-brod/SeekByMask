#include "FileWrapper.h"

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
    str.resize(size);

    void* strDataPtr = (void*)str.data();

    memset(strDataPtr, 0, size);
    fread_s(strDataPtr, size, size, 1, _file);

    str.shrink_to_fit();
    _cursor += str.size();

    return str;
}

std::string FileHandle::ReadLine() {

    assert(isOpen(), "FileHandle::Seek file isn't open");
    memset(_buf, 0, strBufferLen);
    fgets(_buf, strBufferLen, _file);

    assert(_buf[strBufferLen - 2] == 0, "FileHandle::ReadLine error, input line exceeds max byte count ");

    std::string str(_buf);

    _cursor += str.size();

    return str;
}