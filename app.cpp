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
#include "manentity.h"
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
	mpTestSound(nullptr),
	mCash(0),
	mCountdownSec(0.0f),
	mpCurrentHouse(nullptr),
	mpCurrentTarget(nullptr),
	mMsgDisplayTimeSec(0.0f)
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
	initObjects();
	
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

void Application::initObjects()
{
	Settings::setGroup("level");
	std::vector<std::string> lHouseNames = Settings::getStringVector("houses");
	for (const std::string& lrName: lHouseNames)
	{
		std::string lPosKey = "house" + lrName + "_pos";
		std::vector<float> lHousePos = Settings::getFloatVector(lPosKey);
		float lHouseX = getFloatParam(lHousePos, 0) - 800;	//
		float lHouseY = getFloatParam(lHousePos, 1) - 600;	// for typing convenience :)
		HouseEntity* lpNewHouse = new HouseEntity(lHouseX, lHouseY);
		
		std::string lTexKey = "house" + lrName + "_tex";
		std::string lTex = Settings::getString(lTexKey);
		if (lTex.empty())
			lTex = (std::ostringstream() << (1 + int(emscripten_random() * 9.0f))).str();
		lpNewHouse->setTexture(gTextureManager.load("data/tex/house-" + lTex + ".jpg"));
		
		gEntityManager.registerEntity(lpNewHouse);
		lpNewHouse->setName("house_" + lrName);
	}
	
	std::vector<std::string> lMenNames = Settings::getStringVector("men");
	for (const std::string& lrName: lMenNames)
	{
		std::string lPosKey = "man" + lrName + "_pos";
		std::vector<float> lHousePos = Settings::getFloatVector(lPosKey);
		float lManX = getFloatParam(lHousePos, 0) - 800;	//
		float lManY = getFloatParam(lHousePos, 1) - 600;	// for typing convenience :)
		ManEntity* lpNewMan = new ManEntity(lManX, lManY);
		
		gEntityManager.registerEntity(lpNewMan);
	}
}

//------------------------------------------------------------------------------

void Application::addCash(int lAmount)
{
	mCash += lAmount;
}

//------------------------------------------------------------------------------

void Application::setStatusMessage(const std::string &lrText, const std::string& lrText2)
{
	static const float kDisplayTimeSec = Settings::getFloat("general/msg_display_time_sec");
	mStatusMsg  = lrText2.empty() ? std::string() : lrText;
	mStatusMsg2 = lrText2.empty() ? lrText : lrText2;
	mMsgDisplayTimeSec = kDisplayTimeSec;
	if (!lrText2.empty())
		mMsgDisplayTimeSec *= 1.5f;
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
	
	if (mMsgDisplayTimeSec > 0.0f)
	{
		mMsgDisplayTimeSec -= lTimeDeltaSec;
		if (mMsgDisplayTimeSec <= 0.0f)
		{
			mStatusMsg.clear();
			mStatusMsg2.clear();
		}
	}
	
	if (mCountdownSec > 0.0f)
	{
		mCountdownSec -= lTimeDeltaSec;
		if (mCountdownSec <= 0.0f)
			losePassenger();
	}
	
	gVideo.update(lTimeDeltaSec);
}

//------------------------------------------------------------------------------

void Application::render() const
{
	gVideo.clear();
	
	gEntityManager.render();
	
	//gFontManager.renderInWorld("This moves", 50.0f, 50.0f, { 0xFF, 0x80, 0, 0xFF });
	//gFontManager.renderOnScreen("This doesn't", 200.0f, 200.0f, { 0xFF, 0, 0, 0xFF }, FontManager::kAlignLeft);
	
	static const SDL_Colour kWhite = { 0xFF, 0xFF, 0xFF, 0xFF };
	static const SDL_Colour kOrange = { 0xFF, 0x80, 0, 0xFF };
	static const SDL_Colour kYellow = { 0xFF, 0xFF, 0, 0xFF };
	static const SDL_Colour kRed = { 0xFF, 0x40, 0x40, 0xFF };
	
	char lTextBuf[16];
	snprintf(lTextBuf, sizeof(lTextBuf), "FPS: %.1f", gVideo.approxFPS());
	gFontManager.renderOnScreen(lTextBuf, -10.0f, -30.0f, kWhite, FontManager::kAlignRight, FontManager::kAlignBottom);
	
	if (mCountdownSec > 0.0f)
	{
		snprintf(lTextBuf, sizeof(lTextBuf), "%.1f", mCountdownSec);
		gFontManager.renderOnScreen(lTextBuf, -10.0f, -8.0f, kRed, FontManager::kAlignRight, FontManager::kAlignBottom);
	}
	
	//snprintf(lTextBuf, sizeof(lTextBuf), "Spd: %.1f", gpPlayer->speed());
	//gFontManager.renderOnScreen(lTextBuf, 10.0f, -10.0f, kOrange, FontManager::kAlignLeft, FontManager::kAlignBottom);
	//snprintf(lTextBuf, sizeof(lTextBuf), "Dir: %.1f", gpPlayer->rotationRad());
	//gFontManager.renderOnScreen(lTextBuf, 140.0f, -10.0f, kOrange, FontManager::kAlignLeft, FontManager::kAlignBottom);
	
	snprintf(lTextBuf, sizeof(lTextBuf), "$%d", mCash);	// TO DO: pounds or euros :)
	gFontManager.renderOnScreen(lTextBuf, 10.0f, -8.0f, kOrange, FontManager::kAlignLeft, FontManager::kAlignBottom);
	
	static const float kScreenWidth = Settings::getFloat("screen/width");
	//static const float kScreenHeight = Settings::getFloat("screen/height");
	
	if (!mStatusMsg.empty())
		gFontManager.renderOnScreen(mStatusMsg.c_str(), kScreenWidth * 0.5f, -30.0f, kYellow, FontManager::kAlignXCentre,
									FontManager::kAlignBottom);
	if (!mStatusMsg2.empty())
		gFontManager.renderOnScreen(mStatusMsg2.c_str(), kScreenWidth * 0.5f, -8.0f, kYellow, FontManager::kAlignXCentre,
									FontManager::kAlignBottom);
	
	gVideo.flip();
}

//------------------------------------------------------------------------------

HouseEntity* Application::findHouse(const std::string &lrLabel) const
{
	for (Entity* lpEntity: gEntityManager.allEntities())
	{
		HouseEntity* lpHouse = dynamic_cast<HouseEntity*> (lpEntity);
		if (lpHouse == nullptr)
			continue;
		if (lpHouse->name() == "house_" + lrLabel)
			return lpHouse;
	}
	return nullptr;
}

//------------------------------------------------------------------------------

HouseEntity* Application::pickRandomHouse()
{
	static const std::vector<std::string> kDestinations = Settings::getStringVector("level/destinations");
	
	HouseEntity* lpHouse = nullptr;
	do
	{
		int lTargetIndex = emscripten_random() * float(kDestinations.size());
		std::string lTargetName = kDestinations[lTargetIndex];
		lpHouse = findHouse(lTargetName);
	} while (lpHouse == mpCurrentHouse);
	
	ASSERT(lpHouse != nullptr);
	mpCurrentHouse = lpHouse;
	return lpHouse;
}

//------------------------------------------------------------------------------

void Application::losePassenger()
{
	ASSERT(mpCurrentHouse != nullptr);
	setStatusMessage(mpCurrentHouse->getLoseMessage());
	ASSERT(mpCurrentTarget != nullptr);
	mpCurrentTarget->kill();
	mpCurrentTarget = nullptr;
	stopCountdown();
}

//------------------------------------------------------------------------------

void Application::winPassenger(int lCashValue)
{
	ASSERT(mpCurrentHouse != nullptr);
	
	std::string lMsg = mpCurrentHouse->getWinMessage();
	char lBuf[256];
	snprintf(lBuf, sizeof(lBuf), lMsg.c_str(), lCashValue);
	setStatusMessage(lBuf);
	addCash(lCashValue);
	stopCountdown();
	setCurrentTarget(nullptr);	// killed already
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
