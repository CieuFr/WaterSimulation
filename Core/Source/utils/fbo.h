#pragma once

#include "define.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Framebuffer
{
public:

	GLuint _colorTex;

	Framebuffer(int width, int height)
	{
		_width = width;
		_height = height;
		_fbo = 0;
		_colorTex = 0;
		_depthTex = 0;
	}

	int getWidth() const
	{
		return _width;
	}

	int getHeight() const
	{
		return _height;
	}

	bool initialize()
	{
		// ------------------------------------------------------------------------
		// 1- Create textures to hold pixel data
		// ------------------------------------------------------------------------

		
		glGenTextures(1, &_colorTex);
		glBindTexture(GL_TEXTURE_2D, _colorTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glGenTextures(1, &_depthTex);
		glBindTexture(GL_TEXTURE_2D, _depthTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		// ------------------------------------------------------------------------
		// 2- Setup framebuffer object
		// ------------------------------------------------------------------------

		// create fbo
		glCreateFramebuffers(1, &_fbo);

		// setup color buffer
		glNamedFramebufferTexture(_fbo, GL_COLOR_ATTACHMENT0, _colorTex, 0); // level = 0

		// setup depth buffer
		glNamedFramebufferTexture(_fbo, GL_DEPTH_ATTACHMENT, _depthTex, 0); // level = 0

		// check fbo status
		if(glCheckNamedFramebufferStatus(_fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			return false;
		}

		return true;
	}

	//void resize(int width, int height)
	//{
	//	_width = width;
	//	_height = height;

	//	// resize framebuffer textures
	//	glTextureImage2DEXT(_colorTex, GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	//	glTextureImage2DEXT(_depthTex, GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	//}

	void clear()
	{
		// clear framebuffer
		float clearColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glClearNamedFramebufferfv(_fbo, GL_COLOR, 0, clearColor);
		float clearDepth = 1.0f; // max possible depth
		glClearNamedFramebufferfv(_fbo, GL_DEPTH, 0, &clearDepth); // drawbuffer = 0 when using depth
	}

	void bindToDraw()
	{
		// set framebuffer output to correct color attachment
		// this could be done only once at initialization, doing here just to be safe
		GLenum bufs[1];
		bufs[0] = GL_COLOR_ATTACHMENT0;
		glNamedFramebufferDrawBuffers(_fbo, 1, bufs);

		// enable drawing to the framebuffer
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
	}

	void copyToDefault()
	{
		// to display the rendered image on screen, we will read from the framebuffer's output to the default framebuffer
		// this could be done only once at initialization, doing here just to be safe
		glNamedFramebufferReadBuffer(_fbo, GL_COLOR_ATTACHMENT0);
		glNamedFramebufferDrawBuffer(0, GL_BACK);

		// copy framebuffer contents to default framebuffer
		glBlitNamedFramebuffer(_fbo, 0, 0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

private:
	GLsizei _width;
	GLsizei _height;
	GLuint _fbo;
	
	GLuint _depthTex;
};
