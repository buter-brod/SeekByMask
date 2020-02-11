#ifndef SEARCHUTILS_H
#define SEARCHUTILS_H

#include <string>

inline void assert(const bool what, const std::string& str = "") {

    if (!what) {
        throw std::exception(str.c_str());
    }
}

class FileHandle {

public:

    static constexpr size_t strBufferLen = 4096;

    explicit FileHandle(const std::string& fn) : _filename(fn) {
        open();
    }

    ~FileHandle() {
	    if (isOpen()) {
		    close();
	    }
    }

    size_t GetCursor() const {return _cursor;}

    void open() {
        const auto err = fopen_s(&_file, _filename.c_str(), "rb");
        assert(err == 0, "cannot open file, err=" + std::to_string(err));
    }

    bool isOpen() const {
	    return _file;
    }

	void close() {
	    const auto err = fclose(_file);
        _file = nullptr;
        // todo check err and report somehow without throwing?
    }

    void seek(const size_t where) {
        _cursor = where;
        const auto err = fseek(_file, (long)where, SEEK_SET);
        assert(err == 0, "file seek error");
    }

    std::string read(const size_t where, const size_t size) {
        
    	seek(where);

        std::string str;
        str.resize(size);

        void* strDataPtr = (void*)str.data();

    	memset(strDataPtr, 0, size);
    	fread_s(strDataPtr, size, size, 1, _file);

        str.shrink_to_fit();
        _cursor += str.size();

        return str;
    }

    std::string readLine() {

    	memset(_buf, 0, strBufferLen);
        fgets(_buf, strBufferLen, _file);

        std::string str(_buf);
        _cursor += str.size();

        return str;
    }

private:

    std::string _filename;
	
	char _buf[strBufferLen];
    size_t _cursor{0};
	FILE* _file{nullptr};

};

struct PartInfo {

    PartInfo(const size_t start, const size_t end)
        : _start(start), _end(end) {

        _size = _end - _start + 1;
    }

    size_t _start{ 0 };
    size_t _end{ 0 };
    size_t _size{ 0 };
};

struct OccurrenceInfo {

    OccurrenceInfo(const size_t line, const size_t pos, const std::string& str) : _line(line), _pos(pos), _str(str) {}

    size_t _line{ 0 };
    size_t _pos{ 0 };
    std::string _str;
};

#endif