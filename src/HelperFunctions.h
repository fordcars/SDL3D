// Copyright 2015 Carl Hewett

// This file is part of SDL3D.

// SDL3D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// SDL3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with SDL3D. If not, see <http://www.gnu.org/licenses/>.

#ifndef HELPERFUNCTIONS_H_
#define HELPERFUNCTIONS_H_

#include <string>

namespace HelperFunctions
{
	void clearDataOutput();
	void info(const std::string& msg);
	void warn(const std::string& msg);
	void crash(const std::string& msg);
	void checkSDLError(int line = -1);
	template <typename Type> int findInArray(Type element, Type array[], int numberOfElements);
}

#endif /* HELPERFUNCTIONS_H_ */