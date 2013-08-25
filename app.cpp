//------------------------------------------------------------------------------
// app: The main application class, including the main() function.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "app.h"

#include "audiomanager.h"
#include "boundedentity.h"
#include "camera.h"
#include "carentity.h"
#include "entitymanager.h"
#include "fontmanager.h"
#include "houseentity.h"
#include "playercarentity.h"
#include "settings.h"
#include "spriteentity.h"
#include "texturemanager.h"
#include "useful.h"
#include "video.h"

#include <emscripten/emscripten.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <cstdio>
#include <cstdlib>
#include <sstream>

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
	mAreaLeft(0.0f),
	mAreaRight(0.0f),
	mAreaTop(0.0f),
	mAreaBottom(0.0f),
	mpMusic(nullptr),
	mpTestSound(nullptr)
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
	gVideo.shutDown();
	
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
	if (!gVideo.init(kDisplayWidth, kDisplayHeight))
		return false;
	
	gTextureManager.init();
	
	if (!gFontManager.init(gVideo.getDisplaySurface()))
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
	
	initBackground(kDisplayWidth, kDisplayHeight);
	initPlaces();
	
	//CarEntity* lpCar = new CarEntity(50.0f, 50.0f, "red");
	//gEntityManager.registerEntity(lpCar);
	gEntityManager.registerEntity(new PlayerCarEntity(300.0f, 300.0f));
	
	// Set up the times immediately before starting the main loop
	mStartTimeSec = emscripten_get_now() * 0.001f;
	mCurrentTimeSec = mStartTimeSec;
	
	emscripten_set_main_loop(&Application::updateWrapper, 0, 0);
	
	return true;
}

//------------------------------------------------------------------------------

void Application::initBackground(float lDisplayWidth, float lDisplayHeight)
{
	Texture* lpTexture = gTextureManager.load("data/grass.jpg");
	
	float lCentreX = lDisplayWidth * 0.5f;
	float lCentreY = lDisplayHeight * 0.5f;
	
	for (int lXTile = -1; lXTile <= 1; ++lXTile)
		for (int lYTile = -1; lYTile <= 1; ++lYTile)
		{
			float lX = lCentreX + lXTile * lDisplayWidth;
			float lY = lCentreY + lYTile * lDisplayHeight;
			SpriteEntity* lpBackground = new SpriteEntity(lX, lY);
			lpBackground->setName((std::ostringstream() << "background (" << lXTile << ", " << lYTile << ")").str());
			lpBackground->setTexture(lpTexture);
			lpBackground->setBlendEnabled(false);
			gEntityManager.registerEntity(lpBackground);	// must be first
		}
	
	mAreaLeft = -lDisplayWidth;
	mAreaTop = -lDisplayHeight;
	mAreaRight = 2.0f * lDisplayWidth;
	mAreaBottom = 2.0f * lDisplayHeight;
}

//------------------------------------------------------------------------------

void Application::initPlaces()
{
	Settings::setGroup("level");
	std::vector<int> lHouseNumbers = Settings::getIntVector("houses");
	for (int lNum: lHouseNumbers)
	{
		std::string lPosKey = (std::ostringstream() << "house" << lNum << "_pos").str();
		std::vector<float> lHousePos = Settings::getFloatVector(lPosKey);
		HouseEntity* lpNewHouse = new HouseEntity(getFloatParam(lHousePos, 0), getFloatParam(lHousePos, 1));
		gEntityManager.registerEntity(lpNewHouse);
	}
}

//------------------------------------------------------------------------------

void Application::processEvents()
{
	static bool sReturnHeld = false;
	
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
				else if (lEvent.key.keysym.sym == SDLK_RETURN)
				{
					if (!sReturnHeld)
					{
						printf("\n");
						sReturnHeld = true;
					}
				}
				break;
			case SDL_KEYUP:
				if (lEvent.key.keysym.sym == SDLK_RETURN)
					sReturnHeld = false;
				break;
			default:
				break;
		}
}

//------------------------------------------------------------------------------

void Application::update(float lTimeDeltaSec)
{
	gEntityManager.update(lTimeDeltaSec);
	
	gpCamera->updateFromPlayer(gpPlayer, mAreaLeft, mAreaTop, mAreaRight, mAreaBottom);
	
	gVideo.update(lTimeDeltaSec);
}

//------------------------------------------------------------------------------

void Application::render() const
{
	gVideo.clear();
	
	gEntityManager.render();
	
	//gFontManager.renderInWorld("This moves", 50.0f, 50.0f, { 0xFF, 0x80, 0, 0xFF });
	//gFontManager.renderOnScreen("This doesn't", 200.0f, 200.0f, { 0xFF, 0, 0, 0xFF }, FontManager::kAlignLeft);
	
	SDL_Colour kWhite = { 0xFF, 0xFF, 0xFF, 0xFF };
	SDL_Colour kOrange = { 0xFF, 0x80, 0, 0xFF };
	
	char lTextBuf[16];
	snprintf(lTextBuf, sizeof(lTextBuf), "FPS: %.1f", gVideo.approxFPS());
	gFontManager.renderOnScreen(lTextBuf, -10.0f, -10.0f, kWhite, FontManager::kAlignRight, FontManager::kAlignBottom);
	
	snprintf(lTextBuf, sizeof(lTextBuf), "Spd: %.1f", gpPlayer->speed());
	gFontManager.renderOnScreen(lTextBuf, 10.0f, -10.0f, kOrange, FontManager::kAlignLeft, FontManager::kAlignBottom);
	snprintf(lTextBuf, sizeof(lTextBuf), "Dir: %.1f", gpPlayer->rotationRad());
	gFontManager.renderOnScreen(lTextBuf, 140.0f, -10.0f, kOrange, FontManager::kAlignLeft, FontManager::kAlignBottom);
	
	gVideo.flip();
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
