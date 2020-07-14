#pragma once
#include "Shader.h"
class Terrain_Shader :
	public Shader
{
public:
	Terrain_Shader() = default;

	GLuint GetDispFactorLocation();

	GLuint GetDisplacementLocation();

	void SetDirectionalLight(DirectionalLight* dLight);
	void SetPointLight(PointLight* pLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
	void SetSpotLight(SpotLight* sLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
	void SetDirectionalShadowMaps(Light* light, unsigned int i,GLuint textureUnit);
	void SetAOMap(GLuint textureUnit);
	void SetIrradianceMap(GLuint textureUnit);
	void SetPrefilterMap(GLuint textureUnit);
	void SetBRDFLUT(GLuint textureUnit);
	void SetDirectionalLightTransforms(int i, glm::mat4* lTransform);
	void SetDirectionalLightTransform(glm::mat4* lTransform);
	void SetCascadeEndClipSpace(int i, float z);
	void SetLightMatrices(std::vector<glm::mat4> lightMatrices);

	~Terrain_Shader();
private:
	void CompileProgram();
	GLuint uniformDispFactor = 0;
	GLuint uniformDisplacement = 0;
	GLuint uniformDirectionalLightTransforms[NUM_CASCADES] = { 0 };
	GLuint uniformCascadeEndClipSpace[NUM_CASCADES] = { 0 };
	struct {
		GLuint shadowMap;
	} uniformDirectionalShadowMaps[NUM_CASCADES];
};

