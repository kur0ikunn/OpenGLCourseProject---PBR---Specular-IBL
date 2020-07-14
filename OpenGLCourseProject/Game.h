#pragma once

#include <stdio.h>
#include <string>
#include <cmath>
#include <memory>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "CommonValues.h"

#include "Window.h"
#include "Equirectangular_to_CubeMap_Shader.h"
#include "Irradiance_Convolution_Shader.h"
#include "PreFilter_Shader.h"
#include "BRDF_Shader.h"
#include "PreZPass_Shader.h"
#include "Terrain_PreZPass_Shader.h"
#include "SSAO_Shader.h"
#include "SSAOBlur_Shader.h"
#include "Model_Shader.h"
#include "Terrain_Shader.h"
#include "Billboard_Shader.h"
#include "Particle_Shader.h"
#include "HDR_Shader.h"
#include "Blur_Shader.h"
#include "MotionBlur_Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

#include "Equirectangular_to_CubeMap_Framebuffer.h"
#include "PreFilter_Framebuffer.h"
#include "BRDF_Framebuffer.h"
#include "Depth_Framebuffer.h"
#include "SSAO_Framebuffer.h"
#include "SSAOBlur_Framebuffer.h"
#include "HDR_Framebuffer.h"
#include "Motion_Blur_FrameBuffer.h"

#include "Blur_PingPong_Framebuffer.h"

#include "Static_Model.h"
#include "Animated_Model.h"

#include "ParticleSystem.h"
#include "Skybox.h"

#include <assimp/Importer.hpp>



class Game {

public:
	Game();

	void init();

	void update(float fps);
	Window mainWindow = Window(ScreenWidth, ScreenHeight);
	~Game();
private:
	void calcAverageNormals(unsigned int* indices, unsigned int indicesCount,
		GLfloat* vertices, unsigned int verticesCount,
		unsigned int vLength, unsigned int normalOffset);

	void calcAverageTangents(unsigned int* indices, unsigned int indicesCount,
		GLfloat* vertices, unsigned int verticesCount,
		unsigned int vLength, unsigned int tangentOffset);

	void CreateBillboard();
	void CreateParticles();
	void CreateTerrain();
	void CreateObject();
	void CreateShaders();
	void RenderBillboardScene();
	void RenderParticlesScene(GLfloat deltaTime);
	void RenderTerrain();
	void RenderEnvCubeMap(bool is_cubeMap);
	void RenderScene(glm::mat4 projectionMatrix = glm::mat4(), glm::mat4 viewMatrix = glm::mat4());
	void RenderAnimScene(bool shadow, bool depth);

	void EnvironmentMapPass();
	void IrradianceConvolutionPass();
	void PrefilterPass();
	void BRDFPass();
	void PreZPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, GLfloat deltaTime);
	void SSAOPass(glm::mat4 projectionMatrix);
	void SSAOBlurPass();
	void DirectionalShadowMapPass(glm::mat4 viewMatrix, DirectionalLight* light);
	void OmniShadowMapPass(PointLight* light);
	void RenderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, GLfloat deltaTime);
	void MotionBlurPass(float fps);
	void BlurPass();
	void BloomPass();
	const float toRadians = static_cast<float>(M_PI) / 180.0f;

	GLuint uniformProjectionEnv = 0, uniformViewEnv = 0;
	GLuint uniformProjectionIrr = 0, uniformViewIrr = 0;
	GLuint uniformProjectionPreF = 0, uniformViewPreF = 0;
	
	GLuint uniformProjectionAO = 0, uniformSampleRadius = 0;

	GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0, uniformPrevPVM = 0, uniformEyePosition = 0, uniformHeightScale = 0,
	    uniformAlbedoMap = 0, uniformMetallicMap = 0, uniformNormalMap = 0, uniformRoughnessMap = 0, uniformParallaxMap = 0, uniformGlowMap = 0,
		uniformOmniLightPos = 0, uniformFarPlane = 0;

	GLuint uniformModel1 = 0, uniformProjection1 = 0, uniformView1 = 0, uniformPrevPVM1 = 0, uniformEyePosition1 = 0, uniformHeightScale1 = 0,
		uniformAlbedoMap1 = 0, uniformMetallicMap1 = 0, uniformNormalMap1 = 0, uniformRoughnessMap1 = 0, uniformParallaxMap1 = 0, uniformGlowMap1 = 0,
		uniformOmniLightPos1 = 0, uniformFarPlane1 = 0;

	GLuint uniformBones[MAX_BONES] = { 0 };

	GLuint uniformModel2 = 0, uniformProjection2 = 0, uniformView2 = 0, uniformPrevPVM2 = 0, uniformEyePosition2 = 0, uniformDispFactor = 0, uniformDisplacement = 0,
		uniformAlbedoMap2 = 0, uniformMetallicMap2 = 0, uniformRoughnessMap2 = 0,
		uniformOmniLightPos2 = 0, uniformFarPlane2 = 0;

	GLuint uniformProjection0 = 0, uniformView0 = 0, uniformPrevPV0 = 0, uniformCameraUp = 0, uniformCameraRight = 0, uniformPos = 0, uniformSize = 0;

	GLuint uniformProjectionParticles = 0, uniformViewParticles = 0, uniformCameraUpParticles = 0, uniformCameraRightParticles = 0;

	GLuint uniformHDR = 0, uniformExposure = 0, uniformBlur = 0;

	GLuint uniformHorizontal = 0, uniformWeight = 0;

	GLuint uniformVelocityScale = 0;

	glm::mat4 projection = glm::mat4();

	Equirectangular_to_CubeMap_Shader environmentMapShader;
	Equirectangular_to_CubeMap_Framebuffer* environmentMap;

	Irradiance_Convolution_Shader irradianceConvolutionShader;
	Equirectangular_to_CubeMap_Framebuffer* irradianceMap;

	PreFilter_Shader prefilterShader;
	PreFilter_Framebuffer* prefilterMap;

	BRDF_Shader brdfShader;
	BRDF_Framebuffer* brdfMap;

	Model_Shader directionalShadowShader;
	Model_Shader omniShadowShader;

	Model_Shader animDirectionalShadowShader;
	Model_Shader animOmniShadowShader;

	Terrain_Shader terrainDirectionalShadowShader;
	Terrain_Shader terrainOmniDirectionalShadowShader;

	PreZPass_Shader static_preZPassShader;
	PreZPass_Shader anim_preZPassShader;
	Terrain_PreZPass_Shader terrain_preZPassShader;
	Depth_Framebuffer* depth = nullptr;

	SSAO_Shader ssaoShader;
	SSAO_Framebuffer* ssao = nullptr;

	SSAOBlur_Shader ssaoBlurShader;
	SSAOBlur_Framebuffer* ssaoBlur = nullptr;

	std::vector<Model_Shader> shaderList;
	std::vector<Model_Shader> animShaderList;

	Terrain_Shader terrainShader;

	Billboard_Shader billboardShader;
	Particle_Shader particleShader;

	HDR_Shader hdrShader;
	HDR_Framebuffer* hdr = nullptr;
	MotionBlur_Shader motionBlurShader;
	Motion_Blur_FrameBuffer* motionBlur = nullptr;
	Blur_Shader blurShader;
	Blur_PingPong_Framebuffer* blur = nullptr;

	std::vector<Static_Mesh*> meshList;
	std::vector<Static_Mesh*> terrainList;
	std::vector<Static_Mesh*> billboardList;
	std::vector<ParticleSystem*> particleList;

	Static_Mesh* quad;
	Static_Mesh* mesh_cube;

	Camera camera;

	DirectionalLight mainLight;
	PointLight pointLights[MAX_POINT_LIGHTS];
	SpotLight spotLights[MAX_SPOT_LIGHTS];

	unsigned int pointLightCount = 0;
	unsigned int spotLightCount = 0;

	Skybox* skybox;

	Texture environmentTexture;
	Texture skyboxTexture;

	Texture brickTexture;
	Texture brickTextureMetal;
	Texture brickTextureNorm;
	Texture brickTextureRough;
	Texture brickTexturePara;
	Texture brickTextureGlow;
	Texture metalDebrisTexture;
	Texture metalDebrisTextureMetal;
	Texture metalDebrisTextureNorm;
	Texture metalDebrisTextureRough;
	Texture metalDebrisTexturePara;
	Texture metalDebrisTextureGlow;
	Texture floorTexture;
	Texture floorTextureMetal;
	Texture floorTextureNorm;
	Texture floorTextureRough;
	Texture floorTexturePara;
	Texture floorTextureGlow;
	Texture rustedMetal;
	Texture rustedMetalMetal;
	Texture rustedMetalNorm;
	Texture rustedMetalRough;
	Texture rustedMetalPara;
	Texture rustedMetalGlow;

	Texture floorTexture1;
	Texture floorTextureMetal1;
	Texture floorTextureRough1;
	Texture floorTextureDisp1;

	Texture SSAONoiseTexture;

	Texture plainTexture;
	Texture grassTexture;

	Material shinyMaterialGlow;
	Material dullMaterialGlow;
	Material shinyMaterialPara;
	Material dullMaterialPara;
	Material shinyMaterialRough;
	Material dullMaterialRough;
	Material shinyMaterialNorm;
	Material dullMaterialNorm;
	Material shinyMaterialMetal;
	Material dullMaterialMetal;
	Material shinyMaterial;
	Material dullMaterial;

	Material shinyTerrainMaterial;
	Material dullTerrainMaterial;

	Static_Model sniper;
	Static_Model anymodel;
	Static_Model bulb;
	Static_Model cube;
	Static_Model sphere;
	Animated_Model anim, anim2;

	GLfloat deltaTime = 0.0f;
	GLfloat lastTime = 0.0f;

	GLfloat aircraftAngle = 0.0f;

	//Vertex Shader
	const char* vShader = "Shaders/shader.vert";

	//Fragment Shaders
	const char* fShader = "Shaders/shader.frag";

	const char* avShader = "Shaders/animated_shader.vert";

	std::vector<glm::vec3> ssaoNoiseData{ 100, glm::vec3(0.0f, 0.0f, 0.0f) };

	glm::mat4 prevProjView = glm::mat4();

	glm::mat4 prevProj = glm::mat4();
	glm::mat4 prevView = glm::mat4();

	glm::mat4 vView[3] = { glm::mat4() };
	glm::mat4 testLitView[1] = { glm::mat4() };

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[6] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
	};
};