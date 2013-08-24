//------------------------------------------------------------------------------
// Useful: Miscellaneous useful functions.
//
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#ifndef USEFUL_H
#define USEFUL_H

#include <cstdlib>
#include <string>
#include <vector>

//------------------------------------------------------------------------------

#define ASSERT(expr)		if (!(expr)) \
							{ \
								printf("\nASSERTION FAILED at " __FILE__ ":%d: " #expr "\n", __LINE__); \
								abort(); \
							}
#define ASSERT2(expr, msg)	if (!(expr)) \
							{ \
								printf("\nASSERTION FAILED at " __FILE__ ":%d: " #expr "\n%s\n", __LINE__, msg); \
								abort(); \
							}

#define TRACE_HERE()	printf("Now at: " __FILE__ ":%d\n", __LINE__)

#define M_PI_OVER_2 M_PI_2		// still needs cmath

//------------------------------------------------------------------------------

struct SDL_Surface;

//------------------------------------------------------------------------------

//
// String manipulation
//

std::vector<std::string> stringVecFromCharPtrArray(int lArrayLength, char* lpArgV[]);
std::vector<std::string> split(const std::string& lrOriginal, const std::string& lrSplitChars = " \t");	// skips empty parts
std::string stripped(const std::string& lrOriginal);
void strip(std::string& lrTarget);

//
// Functions to use a string vector as a list of parameters
//

const std::string& getStringParam(const std::vector<std::string>& lrParamList, int lParamIndex);
int getIntParam(const std::vector<std::string>& lrParamList, int lParamIndex);
float getFloatParam(const std::vector<std::string>& lrParamList, int lParamIndex);

//
// Other functions
//

void traceDirContents(const std::string& lrDirName);

bool floatApproxEquals(float lVal1, float lVal2);

// Set the loop factor to a value in [0, 1] to cycle the gradient start position
void setSurfaceToGradient(SDL_Surface* lpSurface, float lLoopFactor);

template <typename Type> Type min(Type lVal1, Type lVal2) { return lVal1 < lVal2 ? lVal1 : lVal2; }
template <typename Type> Type max(Type lVal1, Type lVal2) { return lVal1 > lVal2 ? lVal1 : lVal2; }

void getPolarFromRect(float lX, float lY, float* lpMagOut, float* lpAngleRadOut);
void getRectFromPolar(float lMag, float lAngleRad, float* lpXOut, float* lpYOut);

//------------------------------------------------------------------------------

#endif // USEFUL_H
