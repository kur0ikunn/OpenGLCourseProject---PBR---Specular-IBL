#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>

#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonValues.h"

#include "Model_Shader.h"
#include "Static_Mesh.h"

class Skybox
{
public:
	Skybox();

	Skybox(std::vector<std::string> faceLocation);

	void DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::mat4 prevP, glm::mat4 prevV);
	~Skybox();

private:
	Static_Mesh* skyMesh = nullptr;
	Model_Shader* skyShader = nullptr;
	
	GLuint textureId = 0;
	GLuint uniformProjection{ 0 }, uniformView{ 0 }, uniformPrevPV{0};

};

