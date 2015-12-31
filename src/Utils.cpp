//// Copyright 2015 Carl Hewett
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

#include <Utils.hpp>
#include <Definitions.hpp>

#include <SDL.h>
#include <fstream>

#include <sstream> // For std::getLine()

namespace Utils
{
	std::ofstream gLogFile(LOG_FILE, std::ios::app); // Evil global

	void closeLogFile() // Log file opens by itself, but doesn't close by itself
	{
		gLogFile.close();
	}

	void clearDataOutput()
	{
		if(gLogFile.is_open()) // Close the file
		{
			gLogFile.close();
		}

		std::ofstream dataFile(LOG_FILE); // No error checking necessairy
		dataFile << "";
		dataFile.close();

		gLogFile.open(LOG_FILE, std::ios::app); // Reopen the file
	}

	void directly_logprint(const std::string& msg, int line, const char* file)
	{
		gLogFile << msg << '\n';

#ifndef NDEBUG // Debug
		bool parametersValid = false;

		if(file != 0) // If file is defined
		{
			gLogFile << "----- from file: " << file << '\n';
			parametersValid = true;
		}

		if(line != -1) // If line is defined
		{
			gLogFile << "----- at line: " << line << '\n';
			parametersValid = true;
		}

		if(parametersValid) // Add a newline if you outputted something, for prettyness.
			gLogFile << '\n';

		gLogFile.flush(); // Whatever happens, flush out the buffer so we get logs even if it exploded: this is debug!
#endif
	}

	void directly_warn(const std::string& msg, int line, const char* file)
	{
		std::string fullString = "Warning: " + msg; // Concentenate
		directly_logprint(fullString, line, file);
	}

	// This quits the game, so don't expect to be able to do other things after calling this (including logging)!
	void directly_crash(const std::string& msg, int line, const char* file)
	{
		std::string sdlError = SDL_GetError();

		std::string fullString = "Error: " + msg; // Concentenate

		if(sdlError.length()>0)
			fullString += "\nSDL error: " + sdlError; // Info adds a newline at the end of the string

		directly_logprint(fullString, line, file);

		closeLogFile();
		SDL_Quit();
		exit(1); // Not the best
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
			CRASH(crashLog);
			return 0;
		}
	}

	// String splitting
	std::vector<std::string>& splitString(const std::string& s, char delim, std::vector<std::string>& elems)
	{
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}

	std::vector<std::string> splitString(const std::string& s, char delim)
	{
		std::vector<std::string> elems;
		splitString(s, delim, elems);
		return elems;
	}
}