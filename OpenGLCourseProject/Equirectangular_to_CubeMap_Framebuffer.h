#pragma once
#include "Framebuffer.h"
class Equirectangular_to_CubeMap_Framebuffer :
    public Framebuffer
{
public:
	Equirectangular_to_CubeMap_Framebuffer() = default;

	bool Init(GLuint width, GLuint height);
	void Write(int i);
	void Read(GLenum textureUnit);
	
	~Equirectangular_to_CubeMap_Framebuffer();
private:
	GLuint rboDepth=0;
};

