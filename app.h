//------------------------------------------------------------------------------
// app: The main application class, including the main() function.
//
// Convention: all x and y positions are centres.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef APP_H
#define APP_H

#include <string>
#include <vector>

//------------------------------------------------------------------------------

struct SDL_Surface;
class Music;
class Sound;

//------------------------------------------------------------------------------

class Application
{
public:
	Application(const std::vector<std::string>& lrArgs);
	~Application();
	
	static Application& instance();
	
	int run();		// returns a result code: 0 for success, or positive for error codes
	void quit();	// sets a quit flag to be processed on the next update
	
	float getCurrentTimeMS() const;
	SDL_Surface* getDisplaySurface() const;
	
	enum PageUpdateType { UpdatePage, DoNotUpdatePage };
	void toggleMusic(PageUpdateType lUpdate);
	
private:
	
	void processEvents();
	static void updateWrapper();		// just calls msInstance->update()
	void update();						// calls non-static update(float) with time delta
	void update(float lTimeDeltaSec);
	void render() const;
	
	bool init();		// returns false on failure
	void runMainLoopIteration();
	
	
	static Application* msInstance;
	
	const std::vector<std::string> mArgs;
	
	float mStartTimeSec;
	float mCurrentTimeSec;
	bool mQuit;
	
	Music* mpMusic;
	Sound* mpTestSound;
	
	SDL_Surface* mpDisplaySurface;
	SDL_Surface* mpBackground;
	SDL_Surface* mpGuard;
	std::vector<float> mvGuardX;
	std::vector<float> mvGuardY;
	std::vector<float> mvGuardVX;
	std::vector<float> mvGuardVY;
};

#define gApplication Application::instance()

//------------------------------------------------------------------------------

#endif // APP_H
