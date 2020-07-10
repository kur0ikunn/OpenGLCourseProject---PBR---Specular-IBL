#pragma once
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "CommonValues.h"

class Texture
{
public:
	Texture();
	Texture(const char* fileLoc);

	bool LoadTexture();
	bool LoadTextureA();
	bool LoadTextureSRGB();
	bool LoadTextureSRGBA();
	bool LoadCubeMap(std::vector<std::string> faceLocation);
	bool LoadCubeMapSRGB(std::vector<std::string> faceLocation);
	bool LoadTextureHDR();

	bool GenerateNoiseTexture(std::vector<glm::vec3>& noiseData);

	void UseTexture(unsigned int i);
	void UseCubeMap(unsigned int i);

	void ClearTexture();

	~Texture();
private:
	GLuint textureID;
	int width, height, bitDepth;

	const char* fileLocation;
};

