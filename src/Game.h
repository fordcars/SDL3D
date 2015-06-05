#include <GL/glew.h>
#include <SDL_opengl.h>
#include <gl/GLU.h>
#include <SDL.h>

#include <ResourceManager.h>

class Game
{
private:
	std::string mGameName;
	std::string mLogFile;
	std::string mResourceDir;

	int mGameWidth;
	int mGameHeight;

	SDL_Window *mMainWindow;
	SDL_GLContext mMainContext; // OpenGl context

	ResourceManager *mResourceManager;

public:
	Game(const char *gameName, int width, int height, const char *resourceDir);
	~Game();
	void init();
	void quit();

	SDL_Window *getMainWindow();
	SDL_GLContext getMainContext();
	ResourceManager *getResourceManager();
};