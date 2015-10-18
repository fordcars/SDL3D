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
#include <vector>
#include <cstdlib> // For size_t

namespace HelperFunctions
{
	void closeLogFile();
	void clearDataOutput();
	void logprint(const std::string& msg);
	void warn(const std::string& msg);
	void crash(const std::string& msg);
	void checkSDLError(int line = -1);

	std::string getFileContents(const std::string& filePath);

	std::vector<std::string>& splitString(const std::string& s, char delim, std::vector<std::string>& elems);
	std::vector<std::string> splitString(const std::string& s, char delim);

	// Templates
	// The size of the vector's data, in bytes
	template<typename T>
	std::size_t getSizeOfVectorData(const typename std::vector<T> &vec)
	{
		return sizeof(T) * vec.size();
	}

	template<typename T> // Type is used to represent a single datatype. This line is part of the function definition
	int findInArray(T element, T array[], int numberOfElements) // Uses comparision "==" and returns the index of the first instance of the element.
	{
		for(int i = 0; i < numberOfElements; i++)
		{
			if(array[i] == element)
			{
				return i;
			}
		}

		return -1; // Thank god indexes can't be negative!
	}
}

#endif /* HELPERFUNCTIONS_H_ */