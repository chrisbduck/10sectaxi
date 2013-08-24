//------------------------------------------------------------------------------
// app: The main application class, including the main() function.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "app.h"

#include "audiomanager.h"
#include "boundedentity.h"
#include "camera.h"
#include "entitymanager.h"
#include "fontmanager.h"
#include "settings.h"
#include "spriteentity.h"
#include "texturemanager.h"
#include "useful.h"

#include <emscripten/emscripten.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <cstdio>
#include <cstdlib>

//------------------------------------------------------------------------------
// Functions defined in JavaScript in the HTML file
//------------------------------------------------------------------------------

extern "C" {
	extern void page_toggleMusic();
	extern bool page_isMusicEnabled();
}

//------------------------------------------------------------------------------
// Application class
//------------------------------------------------------------------------------

Application* Application::msInstance = nullptr;

//------------------------------------------------------------------------------

Application::Application(const std::vector<std::string> &lrArgs) :
	mArgs(lrArgs),
	mStartTimeSec(0.0f),
	mCurrentTimeSec(0.0f),
	mQuit(false),
	mpMusic(nullptr),
	mpDisplaySurface(nullptr)
{
	ASSERT(msInstance == nullptr);
	msInstance = this;
}

//------------------------------------------------------------------------------

Application::~Application()
{
	printf("Exiting\n");
	
	ASSERT(msInstance != nullptr);
	msInstance = nullptr;
	
	emscripten_cancel_main_loop();
	
	mpMusic = nullptr;		// freed by the audio manager
	
	gEntityManager.shutDown();
	gAudioManager.shutDown();
	gFontManager.shutDown();
	gTextureManager.shutDown();
	
	if (mpDisplaySurface != nullptr)
	{
		SDL_FreeSurface(mpDisplaySurface);
		mpDisplaySurface = nullptr;
	}
	SDL_Quit();
}

//------------------------------------------------------------------------------

Application& Application::instance()
{
	ASSERT(msInstance != nullptr);
	return *msInstance;
}

//------------------------------------------------------------------------------

bool Application::init()
{
	int lInitResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	if (lInitResult	< 0)
	{
		printf("SDL initialisation failed: %d\n", lInitResult);
		return false;
	}
	
	printf("Hello everyone!\n");
	
	const int kDisplayWidth = Settings::getInt("screen/width");
	const int kDisplayHeight = Settings::getInt("screen/height");
	mpDisplaySurface = SDL_SetVideoMode(kDisplayWidth, kDisplayHeight, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (mpDisplaySurface == nullptr)
	{
		printf("Failed to set SDL video mode %dx%dx32\n", kDisplayWidth, kDisplayHeight);
		return false;
	}
	
	SDL_FillRect(mpDisplaySurface, nullptr, 0xFF000000);
	SDL_Flip(mpDisplaySurface);
	
	gTextureManager.init();
	
	if (!gFontManager.init(mpDisplaySurface))
		return false;
	
	gEntityManager.init();
	
	Camera* lpCamera = new Camera(float(kDisplayWidth) * 0.5f, float(kDisplayHeight) * 0.5f,
								  float(kDisplayWidth), float(kDisplayHeight));
	ASSERT(lpCamera == gpCamera);
	
	// Init audio if possible; we can cope without it if needed, though
	gAudioManager.init();
	mpMusic = gAudioManager.loadMusic("data/test_music.ogg");
	if (page_isMusicEnabled())
		mpMusic->play();
	
	//mpTestSound = gAudioManager.loadSound("data/test_sfx.ogg");
	//mpTestSound->play();
	
	mpBackground = gTextureManager.load("data/grass.jpg");
	mpGuard = gTextureManager.load("data/guard.png");
	
	SpriteEntity* lpBackground = new SpriteEntity(float(kDisplayWidth) * 0.5f + 50.0f, float(kDisplayHeight) * 0.5f);
	lpBackground->setName("background");
	lpBackground->setTexture(gTextureManager.load("data/grass.jpg"));
	gEntityManager.registerEntity(lpBackground);	// must be first
	
	const int kNumGuards = Settings::getInt("num_guards");
	for (int lGuardIndex = 0; lGuardIndex < kNumGuards; ++lGuardIndex)
	{
		BoundedEntity* lpEntity = new BoundedEntity(emscripten_random() * 700.0f + 50.0f,
													emscripten_random() * 500.0f + 50.0f);
		gEntityManager.registerEntity(lpEntity);
		lpEntity->setVelX((30.0f + 90.0f * emscripten_random()) * (emscripten_random() >= 0.5f ? 1.0f : -1.0f));
		lpEntity->setVelY((30.0f + 90.0f * emscripten_random()) * (emscripten_random() >= 0.5f ? 1.0f : -1.0f));
	}
	
	// Set up the times immediately before starting the main loop
	mStartTimeSec = emscripten_get_now() * 0.001f;
	mCurrentTimeSec = mStartTimeSec;
	
	emscripten_set_main_loop(&Application::updateWrapper, 0, 0);
	
	return true;
}

//------------------------------------------------------------------------------

void Application::processEvents()
{
	SDL_Event lEvent;
	while (SDL_PollEvent(&lEvent) > 0)
		switch (lEvent.type)
		{
			case SDL_QUIT:
				quit();
				break;
			case SDL_KEYDOWN:
				if (lEvent.key.keysym.sym == SDLK_ESCAPE)
					quit();
				else if (lEvent.key.keysym.sym == SDLK_m)
					toggleMusic(UpdatePage);
				break;
			default:
				break;
		}
}

//------------------------------------------------------------------------------

void Application::update(float lTimeDeltaSec)
{
	static float sNextTraceSec = mStartTimeSec;
	static int sIterations = 0;
	++sIterations;
	if (mCurrentTimeSec >= sNextTraceSec)
	{
		//printf("Hello again, after %d iterations\n", sIterations);
		sNextTraceSec += 2.0f;
		
		static int s_AgainCounter = 0;
		++s_AgainCounter;
		if (s_AgainCounter >= 15)
		{
			quit();
			return;
		}
	}
	
	gpCamera->setPos(gpCamera->x() - 60.0f * lTimeDeltaSec, gpCamera->y());
	
	gEntityManager.update(lTimeDeltaSec);
}

//------------------------------------------------------------------------------

void Application::render() const
{
	//SDL_BlitSurface(mpBackground, nullptr, mpDisplaySurface, nullptr);
	SDL_FillRect(mpDisplaySurface, nullptr, 0xFF000000);
	
	gEntityManager.render();
	
	gFontManager.render("Hello everyone", 50, 50, { 0xFF, 0x80, 0, 0xFF });
	
	SDL_Flip(mpDisplaySurface);
}

//------------------------------------------------------------------------------

void Application::update()
{
	// Process any pending quit request
	if (mQuit)
	{
		delete this;
		return;
	}
	
	float lPrevTimeSec = mCurrentTimeSec;
	mCurrentTimeSec = emscripten_get_now() * 0.001f;
	float lTimeDeltaSec = mCurrentTimeSec - lPrevTimeSec;
	
	processEvents();
	update(lTimeDeltaSec);
	render();
}

//------------------------------------------------------------------------------

void Application::updateWrapper()
{
	if (msInstance != nullptr)		// this can be null in rare cases, such as after exit when the page is reloaded
		msInstance->update();
}

//------------------------------------------------------------------------------

int Application::run()
{
	if (!init())
		return 1;
	
	return 0;
}

//------------------------------------------------------------------------------

void Application::quit()
{
	mQuit = true;
}

//------------------------------------------------------------------------------

SDL_Surface* Application::getDisplaySurface() const
{
	return mpDisplaySurface;
}

//------------------------------------------------------------------------------

void Application::toggleMusic(PageUpdateType lUpdate)
{
	if (mpMusic->isPlaying())
		mpMusic->stop();
	else
		mpMusic->play();
	
	if (lUpdate == UpdatePage)
		page_toggleMusic();
}

//------------------------------------------------------------------------------
// Main function
//------------------------------------------------------------------------------

int main(int lArgC, char* lpArgV[])
{
	// We need to create the app object but not destroy it, because the Emscripten main loop runs after
	// this main function exits.
	Application* lpApp = new Application(stringVecFromCharPtrArray(lArgC, lpArgV));
	return lpApp->run();
}

//------------------------------------------------------------------------------
// Functions exported for use from JavaScript
//------------------------------------------------------------------------------

extern "C"
{
	void app_toggleMusic()
	{
		Application::instance().toggleMusic(Application::DoNotUpdatePage);
	}
}

//------------------------------------------------------------------------------
