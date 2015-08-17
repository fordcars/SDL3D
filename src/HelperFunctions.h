#ifndef HELPERFUNCTIONS_H_
#define HELPERFUNCTIONS_H_

#include <string>

namespace HelperFunctions
{
	void clearDataOutput();
	void info(const std::string &msg);
	void warn(const std::string &msg);
	void crash(const std::string &msg);
	void checkSDLError(int line = -1);
	template <typename Type> int findInArray(Type element, Type array[], int numberOfElements);
}

#endif /* HELPERFUNCTIONS_H_ */