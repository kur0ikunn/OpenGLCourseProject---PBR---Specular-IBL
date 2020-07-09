#pragma once
#include "Shader.h"
class Model_Shader :
	public Shader
{
public:
	Model_Shader() = default;

	void SetDirectionalLight(DirectionalLight* dLight);
	void SetPointLight(PointLight* pLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
	void SetSpotLight(SpotLight* sLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
	void SetSkybox(GLuint textureUnit);
	void SetDirectionalShadowMap(GLuint textureUnit);
	void SetAOMap(GLuint textureUnit);
	void SetDirectionalLightTransform(glm::mat4* lTransform);
	void SetLightMatrices(std::vector<glm::mat4> lightMatrices);

	~Model_Shader();

private:
	void CompileProgram();
};

