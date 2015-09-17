#include <HelperFunctions.h>
#include <Definitions.h>

#include <SDL.h>
#include <iostream>
#include <fstream>

#include <sstream>
#include <vector>

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

namespace HelperFunctions
{
	void clearDataOutput()
	{
		std::ofstream dataFile;
		dataFile.open(LOG_FILE);
		dataFile << "";
		dataFile.close();
	}

	void info(const std::string& msg)
	{
		std::ofstream dataFile;
		dataFile.open(LOG_FILE, std::ios::app);
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
		exit(1); // Not the best, doesn't work?
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

	template <typename Type> // Type is used to represent a single datatype. This line is part of the function definition
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