#ifndef SEARCHUTILS_H
#define SEARCHUTILS_H

#include <string>

inline void assert(const bool what, const std::string& str = "") {

	if (!what) {
		throw std::exception(str.c_str());
	}
}

#endif