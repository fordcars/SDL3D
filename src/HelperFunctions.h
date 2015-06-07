#ifndef HELPERFUNCTIONS_H_
#define HELPERFUNCTIONS_H_

#include <string>

namespace HelperFunctions
{
	void clearDataOutput();
	void info(const std::string &msg);
	void crash(const std::string &msg);
	void checkSDLError(int line = -1);
}

#endif /* HELPERFUNCTIONS_H_ */