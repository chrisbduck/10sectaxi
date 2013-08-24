//------------------------------------------------------------------------------
// Video: Rendering code.
// 
// by Chris Bevan, 2013
//------------------------------------------------------------------------------

#include "video.h"

#include "useful.h"
#include <GLES2/gl2.h>
#include <SDL/SDL_compat.h>
#include <SDL/SDL_surface.h>

//------------------------------------------------------------------------------

Video gVideo;

//------------------------------------------------------------------------------

Video::Video() :
	mInitialised(false),
	mpDisplaySurface(nullptr),
	mShaderProg(0)
{
}

//------------------------------------------------------------------------------

Video::~Video()
{
	if (mInitialised)
		shutDown();
}

//------------------------------------------------------------------------------

static const char* const kpVertexShaderSource =
		"attribute vec4 vPos;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vPos;\n"
		"}\n";

static const char* const kpFragmentShaderSourface =
		"precision mediump float;\n"
		"void main()\n"
		"{\n"
		"	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
		"}\n";

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
	
	mShaderProg = buildProg(kpVertexShaderSource, kpFragmentShaderSourface);
	
	//SDL_FillRect(mpDisplaySurface, nullptr, 0xFF808080);
	//SDL_Flip(mpDisplaySurface);
	
	mInitialised = true;
	return true;
}

//------------------------------------------------------------------------------

void Video::shutDown()
{
	if (!mInitialised)
		return;
	printf("Shutting down video\n");
	
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
	//SDL_FillRect(mpDisplaySurface, nullptr, 0xFF000000);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//------------------------------------------------------------------------------

void Video::flip()
{
	if (mShaderProg != 0)
	{
		static GLfloat lVertices[] = {  0.0f,  0.5f, 0.0f,
								-0.5f, -0.5f, 0.0f,
								 0.5f, -0.5f, 0.0f };
		
		static bool sInit = true;
		static GLuint sBufferID = 0;
		if (sInit)
		{
			glGenBuffers(1, &sBufferID);
			glBindBuffer(GL_ARRAY_BUFFER, sBufferID);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lVertices), lVertices, GL_STATIC_DRAW);
			sInit = false;
		}
		
		glUseProgram(mShaderProg);
		
		glBindBuffer(GL_ARRAY_BUFFER, sBufferID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisableVertexAttribArray(0);
	}
	
	//SDL_Flip(mpDisplaySurface);
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

GLuint Video::buildProg(const char* lpVertShaderSrc, const char* lpFragShaderSrc)
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
	
	return lProgID;
}

//------------------------------------------------------------------------------
