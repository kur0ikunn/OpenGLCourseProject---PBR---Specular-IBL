#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Animated_Model.h"

#include "CommonValues.h"

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class Shader
{
public:
	Shader() = default;

	void CreateFromString(const char* vertexCode, const char* fragmentCode);
	void CreateFromFiles(const char* vertexLocation, const char* fragmentLocation);
	void CreateFromFiles(const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation);
	void CreateFromFiles(const char* vertexLocation, const char* tesscontrolLocation, const char* tessevaluationLocation, const char* fragmentLocation);
	void CreateFromFiles(const char* vertexLocation, const char* tesscontrolLocation, const char* tessevaluationLocation,const char* geometryLocation, const char* fragmentLocation);

	virtual void Validate();

	std::string ReadFile(const char* fileLocation);

	GLuint GetProjectionLocation();
	GLuint GetModelLocation();
	GLuint GetViewLocation();
	GLuint GetPrevPVMLocation();
	GLuint GetAmbientColorLocation();
	GLuint GetDirectionLocation();
	GLuint GetEyePositionLocation();
	GLuint GetHeightScaleLocation();
	GLuint GetAlbedoLocation();
	GLuint GetMetallicLocation();
	GLuint GetNormalLocation();
	GLuint GetRoughnessLocation();
	GLuint GetParallaxLocation();
	GLuint GetGlowLocation();
	GLuint GetOmniLightPosLocation();
	GLuint GetFarPlaneLocation();
	GLuint GetShaderID();

	virtual void SetDirectionalLight(DirectionalLight* dLight);
	virtual void SetPointLight(PointLight* pLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
	virtual void SetSpotLight(SpotLight* sLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
	virtual void SetTexture(GLuint textureUnit);
	virtual void SetSkybox(GLuint textureUnit);
	virtual void SetIrradianceMap(GLuint textureUnit);
	virtual void SetPrefilterMap(GLuint textureUnit);
	virtual void SetBRDFLUT(GLuint textureUnit);
	virtual void SetDirectionalShadowMap(GLuint textureUnit);
	virtual void SetAOMap(GLuint textureUnit);
	virtual void SetDirectionalLightTransform(glm::mat4* lTransform);
	virtual void SetLightMatrices(std::vector<glm::mat4> lightMatrices);

	void UseShader();
	void ClearShader();

	virtual ~Shader() = 0;

protected:
	int pointLightCount = 0;
	int spotLightCount = 0;

	GLuint shaderID = 0, uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformPrevPVM = 0, uniformEyePosition = 0, uniformHeightScale = 0,
		uniformSkybox = 0, uniformIrradiance = 0, uniformPrefilter = 0, uniformBRDF= 0,
		uniformAlbedo= 0, uniformMetallic = 0, uniformNormal = 0, uniformRoughness = 0, uniformParallax = 0, uniformGlow = 0, uniformAO = 0,
		uniformTexture = 0,
		uniformDirectionalLightTransform = 0, uniformDirectionalShadowMap = 0,
		uniformOmniLightPos = 0, uniformFarPlane = 0;


	GLuint uniformLightMatrices[6]{0};
	GLuint uniformBones[MAX_BONES]{0};

	struct {
		GLuint uniformColor;

		GLuint uniformDirection;
	} uniformDirectionalLight;

	GLuint uniformPointLightCount;

	struct {
		GLuint uniformColor;

		GLuint uniformPosition;
	} uniformPointLight[MAX_POINT_LIGHTS];

	GLuint uniformSpotLightCount;

	struct {
		GLuint uniformColor;

		GLuint uniformPosition;

		GLuint uniformDirection;
		GLuint uniformEdge;
	}uniformSpotLight[MAX_SPOT_LIGHTS];

	struct {
		GLuint shadowMap;
		GLuint farPlane;

	}uniformOmniShadowMap[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS];

	void CompileShader(const char* vertexCode, const char* fragmentCode);
	void CompileShader(const char* vertexCode, const char* geometryCode, const char* fragmentCode);
	void CompileShader(const char* vertexCode, const char* tessellationControlCode, const char* tessellationEvaluationCode, const char* fragmentCode);
	void CompileShader(const char* vertexCode, const char* tessellationControlCode, const char* tessellationEvaluationCode,const char* geometryCode, const char* fragmentCode);
	void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);

	virtual void CompileProgram();
};

