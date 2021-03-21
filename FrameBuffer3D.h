#pragma once
#ifndef FRAMEBUFFER_3D
#define FRAMEBUFFER_3D

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class FrameBuffer3D
{
private:
    GLuint m_texture;					// glut texture ID
    GLuint m_buffer;					// FBO index

	float m_width, m_height, m_depth;		// render dimensions

public:
	FrameBuffer3D (glm::vec3 dim) : FrameBuffer3D(dim.x, dim.y, dim.z)
	{
	}

	FrameBuffer3D (float width, float height, float depth) : m_width(width), m_height(height), m_depth(depth)
    {
		// create the texture to use as framebuffer
		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_3D, m_texture);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, (GLsizei)width, (GLsizei)height, (GLsizei)depth, 0, GL_RED, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

		// configure the framebuffer
		glGenFramebuffers(1, &m_buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);
		glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, m_texture, 0, 0);
		//glDrawBuffer(GL_FRONT_AND_BACK);
		//glReadBuffer(GL_NONE);

		// important: set back to default frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    ~FrameBuffer3D ()
    {
		glDeleteFramebuffers(1, &m_buffer);
		glDeleteTextures(1, &m_texture);
    }

    void use () const
	{
		glViewport(0, 0, (GLsizei)m_width, (GLsizei)m_height);
		glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);
		//glBindTexture(GL_TEXTURE_3D, m_texture);
	}

	void setLayer (int layer)
	{
		// TODO: check if unbind and bind if needed
		glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, m_texture, 0, layer);
	}

	GLuint getTexture () const
	{
		return m_texture;
	}
};

#endif // FRAMEBUFFER_3D