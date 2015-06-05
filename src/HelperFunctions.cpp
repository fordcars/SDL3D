#include <HelperFunctions.h>
#include <Definitions.h>

#include <SDL.h>
#include <string>
#include <iostream>
#include <fstream>

#include <sstream>
#include <vector>

// String splitting
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
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

	void info(const char *msg)
	{
		std::ofstream dataFile;
		dataFile.open(LOG_FILE, std::ios::app);
		dataFile << msg << '\n';
		dataFile.close();
	}

	void crash(const char *msg)
	{
		// Od way of doing this
		std::string strMsg(msg); // const char * to string
		std::string sdlError = SDL_GetError();

		std::string fullString = "Error: " + strMsg + "\n"; // Concentenate

		if(sdlError.length()>0)
			fullString += "SDL error: " + sdlError + "\n";

		info(fullString.c_str());

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
				std::string lineString = std::to_string(line); // const char * to string
				lineString = "Line: " + lineString;
				crash(lineString.c_str());
			}

			SDL_ClearError();
		}
#endif
	}
}