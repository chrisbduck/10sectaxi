//------------------------------------------------------------------------------
// Video: Rendering code.
// 
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "video.h"

#include "settings.h"
#include "useful.h"
#include <GLES2/gl2.h>
#include <SDL/SDL_compat.h>
#include <SDL/SDL_surface.h>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

//------------------------------------------------------------------------------

Video gVideo;

//------------------------------------------------------------------------------

Video::Video() :
	mInitialised(false),
	mpDisplaySurface(nullptr)
{
}

//------------------------------------------------------------------------------

Video::~Video()
{
	if (mInitialised)
		shutDown();
}

//------------------------------------------------------------------------------

bool Video::init(int lDisplayWidth, int lDisplayHeight)
{
	ASSERT(!mInitialised);
	
	mpDisplaySurface = SDL_SetVideoMode(lDisplayWidth, lDisplayHeight, 32, SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL);
	if (mpDisplaySurface == nullptr)
	{
		printf("Failed to set SDL video mode %dx%dx32\n", lDisplayWidth, lDisplayHeight);
		return false;
	}
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glViewport(0, 0, lDisplayWidth, lDisplayHeight);
	glDisable(GL_DEPTH_TEST);
	
	mInitialised = true;
	return true;
}

//------------------------------------------------------------------------------

void Video::shutDown()
{
	if (!mInitialised)
		return;
	printf("Shutting down video\n");
	
	for (GLuint lProgID: mShaderProgSet)
		glDeleteProgram(lProgID);
	mShaderProgSet.clear();
	
	if (mpDisplaySurface != nullptr)
	{
		SDL_FreeSurface(mpDisplaySurface);
		mpDisplaySurface = nullptr;
	}
	
	mInitialised = false;
}

//------------------------------------------------------------------------------

void Video::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//------------------------------------------------------------------------------

void Video::flip()
{
	SDL_GL_SwapBuffers();
}

//------------------------------------------------------------------------------

GLuint Video::loadShader(const char* lpSourceText, ShaderType lType)
{
	GLuint lShaderID = glCreateShader(lType == kVertexShader ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	if (lShaderID == 0)
	{
		printf("Failed to create a shader\n");
		return 0;
	}
	
	// Compile the shader
	glShaderSource(lShaderID, 1, &lpSourceText, nullptr);
	glCompileShader(lShaderID);
	
	// Check for errors
	GLint lCompiledStatus;
	glGetShaderiv(lShaderID, GL_COMPILE_STATUS, &lCompiledStatus);
	if (lCompiledStatus == 0)
	{
		GLint lInfoLength = 0;
		glGetShaderiv(lShaderID, GL_INFO_LOG_LENGTH, &lInfoLength);
		if (lInfoLength > 1)
		{
			std::string lLog;
			lLog.resize(lInfoLength);
			glGetShaderInfoLog(lShaderID, lInfoLength, nullptr, &lLog[0]);
			printf("%s shader compilation error:\n%s\n", lType == kVertexShader ? "Vertex" : "Fragment", lLog.c_str());
		}
		glDeleteShader(lShaderID);
		return 0;
	}
	
	return lShaderID;
}

//------------------------------------------------------------------------------

GLuint Video::buildShaderProg(const char* lpVertShaderSrc, const char* lpFragShaderSrc)
{
	GLuint lVertShaderID = loadShader(lpVertShaderSrc, kVertexShader);
	GLuint lFragShaderID = loadShader(lpFragShaderSrc, kFragmentShader);
	if (lVertShaderID == 0 || lFragShaderID == 0)
		return 0;
	
	GLuint lProgID = glCreateProgram();
	if (lProgID == 0)
	{
		printf("Failed to create a shader prog\n");
		return 0;
	}
	
	// Attach and link
	glAttachShader(lProgID, lVertShaderID);
	glAttachShader(lProgID, lFragShaderID);
	glBindAttribLocation(lProgID, 0, "vPos");
	glLinkProgram(lProgID);
	
	// Check for errors
	GLint lLinkStatus;
	glGetProgramiv(lProgID, GL_LINK_STATUS, &lLinkStatus);
	if (lLinkStatus == 0)
	{
		GLint lInfoLength = 0;
		glGetProgramiv(lProgID, GL_INFO_LOG_LENGTH, &lInfoLength);
		if (lInfoLength > 0)
		{
			std::string lLog;
			lLog.resize(lInfoLength);
			glGetProgramInfoLog(lProgID, lInfoLength, nullptr, &lLog[0]);
			printf("Shader prog link error:\n%s\n", lLog.c_str());
		}
		glDeleteProgram(lProgID);
		return 0;
	}
	
	mShaderProgSet.insert(lProgID);
	return lProgID;
}

//------------------------------------------------------------------------------

GLuint Video::buildShaderProgFromFiles(const std::string& lrVertShaderFileName, const std::string& lrFragShaderFileName)
{
	printf("Building shader programme from %s and %s\n", lrVertShaderFileName.c_str(), lrFragShaderFileName.c_str());
	
	std::string lVertShaderSrc = getFileContents("data/shaders/" + lrVertShaderFileName);
	std::string lFragShaderSrc = getFileContents("data/shaders/" + lrFragShaderFileName);
	
	return buildShaderProg(lVertShaderSrc.c_str(), lFragShaderSrc.c_str());
}

//------------------------------------------------------------------------------
