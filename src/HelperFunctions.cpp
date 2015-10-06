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

#include <HelperFunctions.hpp>
#include <Definitions.hpp>

#include <SDL.h>
#include <fstream>

#include <sstream> // For std::getLine()

namespace HelperFunctions
{
	// String splitting
	std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems) {
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}

	std::vector<std::string> split(const std::string& s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	}

	// The size of the vector's data, in bytes
	template<typename T>
	std::size_t sizeOfVectorData(const typename std::vector<T> &vec)
	{
		return sizeof(T) * vec.size();
	}

	void clearDataOutput()
	{
		std::ofstream dataFile(LOG_FILE); // No error checking necessairy
		dataFile << "";
		dataFile.close();
	}

	void info(const std::string& msg)
	{
		std::ofstream dataFile(LOG_FILE, std::ios::app); // No error checking necessairy
		dataFile << msg << '\n';
		dataFile.close();
	}

	void warn(const std::string& msg)
	{
		std::string fullString = "Warning: " + msg; // Concentenate
		info(fullString);
	}

	void crash(const std::string& msg) // This quits the game, so don't expect to be able to do other things after calling this!
	{
		std::string sdlError = SDL_GetError();

		std::string fullString = "Error: " + msg; // Concentenate

		if(sdlError.length()>0)
			fullString += "\nSDL error: " + sdlError; // Info adds a newline at the end of the string

		info(fullString);

		SDL_Quit();
		exit(1); // Not the best
	}

	void checkSDLError(int line) // Default parameter in HelperFunctions.h
	{
#ifndef NDEBUG
		const char *error = SDL_GetError();

		if(*error!='\0')
		{
			crash(error);
			if(line!=-1)
			{
				std::string lineString = std::to_string(line); // int to string
				lineString = "Line: " + lineString;
				crash(lineString);
			}

			SDL_ClearError();
		}
#endif
	}

	// Does checks and returns the contents of the file
	std::string getFileContents(const std::string& filePath)
	{
		std::ifstream fileStream(filePath, std::ios::in | std::ios::binary);
		if(fileStream)
		{
			std::string contents;

			fileStream.seekg(0, std::ios::end);
			contents.resize((int)fileStream.tellg());
			fileStream.seekg(0, std::ios::beg);
			fileStream.read(&contents[0], contents.size());
			fileStream.close();
			return(contents);
		} else // Can't open the file!
		{
			std::string crashLog = filePath + " doesn't exist or cannot be opened!";
			crash(crashLog);
			return 0;
		}
	}

	template<typename Type> // Type is used to represent a single datatype. This line is part of the function definition
	int findInArray(Type element, Type array[], int numberOfElements) // Uses comparision "==" and returns the index of the first instance of the element.
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