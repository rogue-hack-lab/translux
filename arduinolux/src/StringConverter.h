#include <string>

namespace StringConverter{
	inline static char* ExtractCharArrayFromString(std::string extractFrom) {
		char *cstr = new char[extractFrom.length() + 1];
		strcpy(cstr, extractFrom.c_str());

		return cstr;
	}
}