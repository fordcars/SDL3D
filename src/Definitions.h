#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#define LOG_FILE "Log.txt"
#define MAX_GL_ARRAY_LENGTH 2000

// Texture types
#define BMP_TEXTURE 1
#define DDS_TEXTURE 2

// For DDS files
#define FOURCC_DXT1 0x31545844
#define FOURCC_DXT3 0x33545844
#define FOURCC_DXT5 0x35545844

#include <GLAD/glad.h>
#include <array>
#include <unordered_map>

// Useful typedefs
typedef std::array<GLfloat, MAX_GL_ARRAY_LENGTH> GLfloatArray;
typedef std::unordered_map<std::string, GLuint> GLuintMap;
typedef std::pair<std::string, GLuint> GLuintMapPair;

#endif /* DEFINITIONS_H_ */