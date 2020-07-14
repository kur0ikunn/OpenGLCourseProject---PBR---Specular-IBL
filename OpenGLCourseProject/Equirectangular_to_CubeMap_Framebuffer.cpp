#include "Equirectangular_to_CubeMap_Framebuffer.h"

bool Equirectangular_to_CubeMap_Framebuffer::Init(GLuint width, GLuint height, bool is_env_map)
{
	src_width = width; src_height = height;

	glGenFramebuffers(1, &FBO);
	glGenRenderbuffers(1, &rboDepth);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, src_width, src_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	glGenTextures(1, &buffer);
	glBindTexture(GL_TEXTURE_CUBE_MAP, buffer);

	for (size_t i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, src_width, src_height, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	if(is_env_map) glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	else glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebufer error: %i\n", status);
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void Equirectangular_to_CubeMap_Framebuffer::Write(int i)
{	if(i ==-1) glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	else glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, buffer, 0);
}

void Equirectangular_to_CubeMap_Framebuffer::Read(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, buffer);
}

void Equirectangular_to_CubeMap_Framebuffer::CreateFirstMipFace()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, buffer);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

Equirectangular_to_CubeMap_Framebuffer::~Equirectangular_to_CubeMap_Framebuffer()
{
	if (FBO) {
		glDeleteFramebuffers(1, &FBO);
	}
	if (rboDepth) {
		glDeleteBuffers(1, &rboDepth);
	}
	if (buffer)
	{
		glDeleteTextures(1, &buffer);
	}
}
