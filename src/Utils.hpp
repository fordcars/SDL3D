//// Copyright 2016 Carl Hewett
////
//// This file is part of SDL3D.
////
//// SDL3D is free software: you can redistribute it and/or modify
//// it under the terms of the GNU General Public License as published by
//// the Free Software Foundation, either version 3 of the License, or
//// (at your option) any later version.
////
//// SDL3D is distributed in the hope that it will be useful,
//// but WITHOUT ANY WARRANTY; without even the implied warranty of
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// GNU General Public License for more details.
////
//// You should have received a copy of the GNU General Public License
//// along with SDL3D. If not, see <http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

// -1 for undefined lines, 0 for undefined files

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <string>
#include <vector>
#include <cstdlib> // For std::size_t

// Macros simply replaces text
#define LOGPRINT(msg) directly_logprint(msg) // Don't send the line and file for logprints, it would annoying
#define LOGPRINT_DEBUG(msg) directly_logprint(msg, __LINE__, __FILE__) // But do whatever you want, fam

#define WARN(msg) directly_warn(msg, __LINE__, __FILE__)
#define CRASH(msg) directly_crash(msg, __LINE__, __FILE__)

#define CRASH_FROM_SDL(msg) directly_crashFromSDL(msg, __LINE__, __FILE__)

namespace Utils
{
	void closeLogFile();
	void clearDataOutput();

	// Use macros above to access these
	void directly_logprint(const std::string& msg, int line = -1, const char *file = 0);
	void directly_warn(const std::string& msg, int line = -1, const char *file = 0);
	void directly_crash(const std::string& msg, int line = -1, const char *file = 0);
	void directly_crashFromSDL(const std::string& msg, int line = -1, const char *file = 0);

	std::string getFileContents(const std::string& filePath);

	std::vector<std::string>& splitString(const std::string& s, char delim, std::vector<std::string>& elems);
	std::vector<std::string> splitString(const std::string& s, char delim);

	// Template functions
	// Returns the size of the vector's data, in bytes
	// std::size_t seems to be the way most people use size_t
	template<typename T>
	std::size_t getSizeOfVectorData(const typename std::vector<T> &vec)
	{
		return sizeof(T) * vec.size();
	}

	// Constructs a vector from 'second' values in a map
	// The result is placed in the 'resultVector'
	template<typename mapT, typename vectorT>
	void constructVectorFromMap(const mapT& map, vectorT& resultVector)
	{
		// Using range-based for loop for fun
		// Using auto to support const and non-const iterators
		// (though it has to be const since it is passed as a const reference, so using auto for fun)
		for(const auto& it : map)
			resultVector.push_back(it.second);
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

	// Will generate a fancy unique string key for a map pair
	// Will increment id, so use an id variable specifically for this.
	// Use info to include a custom string into the generated key
	template<typename mapT>
	std::string generateUniqueMapStringKey(mapT map, int& id, const std::string& info = "")
	{
		std::string generatedName = "generatedkey_(" + info + ")_" + std::to_string(id);

		// If the name already exists (very rare), throw a warning and try again
		if(map.find(generatedName) != map.end())
		{
			WARN("Generated key '" + generatedName + "' already exists in map! Please rename your map elements. Will retry and continue.");
			id++; // Make sure it's not the same name!

			return generateUniqueMapStringKey(map, id, info); // Recursion!
		}

		id++; // Make sure it is a new name next time
		return generatedName;
	}
}

#endif /* UTILS_HPP_ */