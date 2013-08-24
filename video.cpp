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
	
	std::string lVertShaderFileName = "data/shaders/" + Settings::getString("shader/vertex");
	std::string lFragShaderFileName = "data/shaders/" + Settings::getString("shader/fragment");
	
	std::string lVertShaderSrc = getFileContents(lVertShaderFileName);
	std::string lFragShaderSrc = getFileContents(lFragShaderFileName);
	
	mShaderProg = buildProg(lVertShaderSrc.c_str(), lFragShaderSrc.c_str());
	
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

#include "app.h"

void Video::flip()
{
	if (mShaderProg != 0)
	{
		/*static GLfloat lVertices[] = {   0.0f,  0.5f, 0.0f,
										-0.5f, -0.5f, 0.0f,
										 0.5f, -0.5f, 0.0f };*/
		/*static GLfloat lVertices[] = {   100.0f,  500.0f, 0.0f,
										 100.0f,  100.0f, 0.0f,
										 700.0f,  500.0f, 0.0f,
										 700.0f,  100.0f, 0.0f };*/
		static GLfloat lVertices[] = {	-0.5f,  0.5f, 0.0f,
										-0.5f, -0.5f, 0.0f,
										 0.5f,  0.5f, 0.0f,
										 0.5f, -0.5f, 0.0f };
		
		static GLfloat lVertUVs[] = { 0.0f, 1.0f,
									  0.0f, 0.0f,
									  1.0f, 1.0f,
									  1.0f, 0.0f };
		
		static bool sInit = true;
		static GLuint sBufferIDs[2] = { 0, 0 };
		static GLuint& srVertBufferID = sBufferIDs[0];
		static GLuint& srVertUVID = sBufferIDs[1];
		if (sInit)
		{
			glGenBuffers(2, sBufferIDs);
			glBindBuffer(GL_ARRAY_BUFFER, srVertBufferID);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lVertices), lVertices, GL_STATIC_DRAW);
			
			glBindBuffer(GL_ARRAY_BUFFER, srVertUVID);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lVertUVs), lVertUVs, GL_STATIC_DRAW);
			
			sInit = false;
		}
		
		glUseProgram(mShaderProg);
		
		GLint lPosID = glGetAttribLocation(mShaderProg, "a_VertPos");
		GLint lUVID = glGetAttribLocation(mShaderProg, "a_VertUV");
		GLint lMatID = glGetUniformLocation(mShaderProg, "u_Matrix");
		GLint lColID = glGetUniformLocation(mShaderProg, "u_Colour");
		GLint lSmpID = glGetUniformLocation(mShaderProg, "u_Texture");
		
		float lScaleX = Settings::getFloat("test/width");
		float lScaleY = Settings::getFloat("test/height");
		float lPosX = Settings::getFloat("test/x");
		float lPosY = Settings::getFloat("test/y");
		float lRotRad = Settings::getFloat("test/rotation") * M_PI / 180.0f;
		
		float lRescalerX = 2.0f / Settings::getFloat("screen/width");
		float lRescalerY = -2.0f / Settings::getFloat("screen/height");
		
		glm::mat4 m(1.0f);	// identity
		m = glm::translate(m, glm::vec3(lPosX * lRescalerX - 1.0f,
										lPosY * lRescalerY + 1.0f,
										0.0f));
		m = glm::rotate(m, lRotRad, glm::vec3(0.0f, 0.0f, 1.0f));
		m = glm::scale(m, glm::vec3(lScaleX * lRescalerX,
									lScaleY * lRescalerY,
									1.0f));
		//m = glm::ortho(0, Settings::getInt("screen/width"), Settings::getInt("screen/height"), 0);
		glUniformMatrix4fv(lMatID, 1, GL_FALSE, &m[0][0]);
		
		float lCol[] = { 0.0f, 1.0f, 0.0f, 1.0f };
		glUniform4fv(lColID, 1, lCol);
		
		SDL_Surface* lpSurf = gApplication.getBackgroundTexture();
		static GLuint sTexID = 0;
		if (lpSurf != nullptr)
		{
			if (sTexID == 0)
			{
				glGenTextures(1, &sTexID);
				glBindTexture(GL_TEXTURE_2D, sTexID);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, lpSurf->w, lpSurf->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, lpSurf->pixels);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, sTexID);
			glUniform1i(lSmpID, 0);
			
			glBindBuffer(GL_ARRAY_BUFFER, srVertUVID);
			glVertexAttribPointer(lUVID, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(lUVID);
		}
		
		glDisable(GL_DEPTH_TEST);
		
		glBindBuffer(GL_ARRAY_BUFFER, srVertBufferID);
		glVertexAttribPointer(lPosID, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(lPosID);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glDisableVertexAttribArray(lPosID);
		
		if (lpSurf != nullptr)
			glDisableVertexAttribArray(lUVID);
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
