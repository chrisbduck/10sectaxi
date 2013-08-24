//------------------------------------------------------------------------------
// Useful: Miscellaneous useful functions.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "useful.h"

#include <SDL/SDL_surface.h>
#include <sys/types.h>
#include <cmath>
#include <dirent.h>

//------------------------------------------------------------------------------
// String manipulation
//------------------------------------------------------------------------------

std::vector<std::string> stringVecFromCharPtrArray(int lArrayLength, char* lpArgV[])
{
	std::vector<std::string> lVec;
	
	for (int lArrayIndex = 0; lArrayIndex < lArrayLength; ++lArrayIndex)
		lVec.push_back(lpArgV[lArrayIndex]);
	
	return lVec;
}

//------------------------------------------------------------------------------

std::vector<std::string> split(const std::string &lrOriginal, const std::string& lrSplitChars)
{
	std::vector<std::string> lVec;
	size_t lEnd = lrOriginal.length();
	size_t lSplitStart = 0;
	bool lFinished = false;
	while (!lFinished)
	{
		size_t lNextSplit = lrOriginal.find_first_of(lrSplitChars, lSplitStart);
		if (lNextSplit == std::string::npos)
		{
			lNextSplit = lEnd;
			lFinished = true;
		}
		if (lNextSplit > lSplitStart)
			lVec.push_back(lrOriginal.substr(lSplitStart, lNextSplit));
		lSplitStart = lNextSplit + 1;
	}
	
	return lVec;
}

//------------------------------------------------------------------------------

void strip(std::string &lrTarget)
{
	std::string::iterator liStart = lrTarget.begin();
	std::string::iterator liEnd = lrTarget.end();
	const std::string kWhitespace(" \t\n");
	while (liStart != liEnd && kWhitespace.find(*liStart) != std::string::npos)
		++liStart;
	while (liEnd != liStart && kWhitespace.find(*(liEnd - 1)) != std::string::npos)
		--liEnd;
	
	lrTarget.erase(liEnd, lrTarget.end());
	lrTarget.erase(lrTarget.begin(), liStart);
}

//------------------------------------------------------------------------------

std::string stripped(const std::string &lrOriginal)
{
	std::string lCopy(lrOriginal);
	strip(lCopy);
	return lCopy;
}

//------------------------------------------------------------------------------
// Functions to use a string vector as a list of parameters
//------------------------------------------------------------------------------

const std::string& getStringParam(const std::vector<std::string> &lrParamList, int lParamIndex)
{
	if (lParamIndex < 0 || lParamIndex >= int(lrParamList.size()))
	{
		static const std::string kEmpty;
		return kEmpty;
	}
	return lrParamList[lParamIndex];
}

//------------------------------------------------------------------------------

int getIntParam(const std::vector<std::string> &lrParamList, int lParamIndex)
{
	return atoi(getStringParam(lrParamList, lParamIndex).c_str());
}

//------------------------------------------------------------------------------

float getFloatParam(const std::vector<std::string>& lrParamList, int lParamIndex)
{
	return atof(getStringParam(lrParamList, lParamIndex).c_str());
}

//------------------------------------------------------------------------------
// Other functions
//------------------------------------------------------------------------------

// A simple example of using libc to list the contents of a directory, grabbed from the net
void traceDirContents(const std::string& lrDirName)
{
	DIR* pDir = opendir(lrDirName.c_str());
	if (pDir == nullptr)
	{
		fprintf(stderr, "Couldn't open directory \"%s\"\n", lrDirName.c_str());
		return;
	}
	
	while (true)
	{
		dirent* pEntry = readdir(pDir);
		if (pEntry == nullptr)
			break;
		puts(pEntry->d_name);
	}
	closedir(pDir);
}

//------------------------------------------------------------------------------

bool floatApproxEquals(float lVal1, float lVal2)
{
	static const float kEpsilon = 1.0e-6f;
	return lVal1 - kEpsilon < lVal2 && lVal2 < lVal1 + kEpsilon;
}

//------------------------------------------------------------------------------

void setSurfaceToGradient(SDL_Surface* lpSurface, float lLoopFactor)
{
	SDL_LockSurface(lpSurface);
	
	const int kWidth = lpSurface->w;
	const int kHeight = lpSurface->h;
	
	unsigned int* pDest = static_cast<unsigned int*> (lpSurface->pixels);
	for (int lY = 0; lY < kHeight; ++lY)
	{
		float lPercentWhite = fmodf(float(lY) / float(kHeight - 1) + lLoopFactor, 1.0f);
		unsigned char lByteVal = static_cast<unsigned char>(255.0f * lPercentWhite);
		unsigned int lS32Val = 0xFF000000 + (lByteVal << 16) + (lByteVal << 8) + lByteVal;
		for (int lX = 0; lX < kWidth; ++lX)
			*pDest++ = lS32Val;
	}
	
	SDL_UnlockSurface(lpSurface);
}

//------------------------------------------------------------------------------

void getPolarFromRect(float lX, float lY, float *lpMagOut, float *lpAngleRadOut)
{
	*lpMagOut = sqrtf(lX * lX + lY * lY);
	
	float lAngleRad;
	bool lXZero = floatApproxEquals(lX, 0.0f);
	bool lYZero = floatApproxEquals(lY, 0.0f);
	if (lXZero || lYZero)
	{
		if (lXZero && lYZero)
			lAngleRad = 0.0f;	// not sure what else to do
		else if (lXZero)
			lAngleRad = (lY > 0.0f) ? M_PI_OVER_2 : -M_PI_OVER_2;
		else	// lYZero
			lAngleRad = (lX > 0.0f) ? 0.0f : M_PI;
	}
	else
	{
		lAngleRad = atanf(lY / lX);
		// x < 0, y > 0: pi + atan
		// x < 0, y < 0: -pi + atan
		if (lX < 0.0f)
		{
			if (lY > 0.0f)
				lAngleRad += M_PI;
			else
				lAngleRad -= M_PI;
		}
	}
	
	*lpAngleRadOut = lAngleRad;
}

//------------------------------------------------------------------------------

void getRectFromPolar(float lMag, float lAngleRad, float *lpXOut, float *lpYOut)
{
	*lpXOut = cosf(lAngleRad) * lMag;
	*lpYOut = sinf(lAngleRad) * lMag;
}

//------------------------------------------------------------------------------
