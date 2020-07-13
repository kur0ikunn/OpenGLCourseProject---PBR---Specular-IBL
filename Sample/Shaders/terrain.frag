#version 460												
const int NUM_CASCADES = 3;
											
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

in vec4 DirectionalLightSpacePos[NUM_CASCADES];
in float ClipSpacePosZ;

in vec4 ClipSpacePos;
in vec4 PrevClipSpacePos;

layout(early_fragment_tests) in;
layout(location = 0)out vec4 color;		
layout(location = 1)out vec4 BrightColor;		
layout(location = 2)out vec4 MotionVector;								

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;
const float PI = 3.14159265359;

vec4 CascadeIndicator = vec4(0.0, 0.0, 0.0, 0.0);

vec3 albedo;
float metallic;
float roughness;
vec3 N;
vec3 V;
vec3 F0 = vec3(0.04);

struct Light
{
	vec3 color;
};

struct DirectionalLight
{
	Light base;
	vec3 direction;
};

struct PointLight
{
	Light base;
	vec3 position;
};

struct SpotLight
{
	PointLight base;
	vec3 direction;
	float edge;
};

struct OmniShadowMap
{
	samplerCube shadowMap;
	float farPlane;
};

struct DirectionalShadowMaps
{
	sampler2D shadowMap;
};

struct Material
{
    sampler2D roughnessMap;
	sampler2D metallicMap;
	sampler2D albedoMap;
};

uniform int PointLightCount;
uniform int SpotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform float CascadeEndClipSpace[NUM_CASCADES];
uniform DirectionalShadowMaps directionalShadowMaps[NUM_CASCADES];
uniform samplerCube skybox;
uniform sampler2D AOMap;
uniform OmniShadowMap omniShadowMaps[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS];

uniform Material material;

uniform vec3 eyePosition;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

vec2 CalcScreenTexCoord()
{
    return gl_FragCoord.xy / vec2(1920, 1080);
}


float CalcDirectionalShadowFactor(int CascadeIndex, DirectionalLight light, vec4 LightSpacePos)
{
	vec3 projCoords = LightSpacePos.xyz/LightSpacePos.w;
	projCoords = (projCoords * 0.5) + 0.5;
	
	float current = projCoords.z;

	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(light.direction);
	
	float bias = 0;
	if(CascadeIndex>=2) bias = max(0.001 * (1 - dot(normal, lightDir)), 0.001);
	else bias = max(0.0001 * (1 - dot(normal, lightDir)), 0.0001);

		
	float shadow = 0.0;
	
	vec2 texelSize = 1.0/textureSize(directionalShadowMaps[CascadeIndex].shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(directionalShadowMaps[CascadeIndex].shadowMap, projCoords.xy + vec2(x,y)* texelSize).r;
			shadow += current - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	
	shadow/=9.0;
	
	if(projCoords.z > 1.0)
	{
		shadow = 0.0;
	}
	
	return shadow;
}

float CalcOmniShadowFactor(PointLight light, int shadowIndex, float bias)
{
	vec3 fragToLight = FragPos - light.position;
	float current = length(fragToLight);
	
	float shadow = 0.0;
	int samples = 20;
	
	float viewDistance = length(eyePosition - FragPos);
	float diskRadius = (1.0 + (viewDistance/omniShadowMaps[shadowIndex].farPlane))/25.0;
	
	for(int i = 0; i< samples; i++)
	{
		float closest = texture(omniShadowMaps[shadowIndex].shadowMap, fragToLight + sampleOffsetDirections[i]*diskRadius).r;
		closest *= omniShadowMaps[shadowIndex].farPlane;
		if(current - bias > closest)
		{
			shadow +=1.0;
		}
	}
	
	shadow/= float(samples);

	return shadow;
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N,H), 0.0);  //0.0
	float NdotH2 = NdotH*NdotH;
	
	float nom = a2;
	float denom = (NdotH2*(a2-1.0)+1.0);
	denom = PI*denom*denom;
	
	return nom/denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness +1.0);
	float k = (r*r)/8.0;
	
	float nom = NdotV;
	float denom = NdotV* (1.0-k)+k;
	
	return nom/denom;
}

float GeometrySmith(vec3 N,vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V),0.0);    //0.0
	float NdotL = max(dot(N, L), 0.0);   //0.0
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
	return ggx1*ggx2;
} 

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return max(F0+(1.0-F0)*pow(1.0-min(cosTheta,1.0), 5.0),0.0);
	//return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}


vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	return max(F0+(max(vec3(1.0-roughness),F0)-F0)*pow(1.0-min(cosTheta,1.0), 5.0),0.0);
	//return F0+(max(vec3(1.0-roughness),F0)-F0)*pow(1.0-min(cosTheta,1.0), 5.0);
}


vec4 CalcLightByDirection(Light light, vec3 direction, float shadowFactor, bool is_DirectionLight, bool is_PointLight, bool is_SpotLight)
{
	float distance = 0.0f;

	distance= length(direction);
	vec3 L = normalize(direction);

	vec3 H = normalize(V+L);
	float attenuation = 1.0/(distance*distance);
	vec3 radiance = light.color*attenuation;
	
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);    //0.0
	
	vec3 nominator = NDF*G*F;
	float denominator = 4* max(dot(N,V),2.0)*max(dot(N,L), 0.0);
	vec3 specular = nominator/max(denominator,0.001);    //0.001-has banding issues 
	
	vec3 kS = F;
	vec3 kD = vec3(1.0)-kS;
	kD*=1.0-metallic;
	
	float NdotL = max(dot(N, L),0.0);
	
	if(is_DirectionLight) radiance = light.color;
	vec3 Lo =(kD*albedo/PI+specular)*radiance* NdotL;
	
	if(light.color.x+light.color.y+light.color.z == 0.0) return vec4(0.0f);	
	else return vec4((1.0-shadowFactor)*Lo, 1.0);
}

vec4 CalcDirectionalLight()
{
	float shadowFactor = 0.0;

	for (int i = 0 ; i < NUM_CASCADES ; i++) {
     if (ClipSpacePosZ<= CascadeEndClipSpace[i]) 
	 {
	 //int i = 0;
        shadowFactor = CalcDirectionalShadowFactor(i, directionalLight, DirectionalLightSpacePos[i]);
		  if (i == 0) 
                CascadeIndicator = vec4(0.1, 0.0, 0.0, 0.0);
            else if (i == 1)
                CascadeIndicator = vec4(0.0, 0.1, 0.0, 0.0);
            else if (i == 2)
                CascadeIndicator = vec4(0.0, 0.0, 0.1, 0.0);
       break;
     }
	}	
	return CalcLightByDirection(directionalLight.base, directionalLight.direction, shadowFactor, true, false, false);
}

vec4 CalcPointLight(PointLight pLight, int shadowIndex, bool is_PointLight, bool is_SpotLight)
{
	vec3 direction = FragPos - pLight.position;
	float distance = length(direction);
	direction = normalize(direction);
	
	vec3 normal = normalize(Normal);
	float bias = max(0.5 * (1 - dot(normal, direction)), 0.1);
	
	float shadowFactor = CalcOmniShadowFactor(pLight, shadowIndex, bias);
		
	vec4 color = CalcLightByDirection(pLight.base, direction, shadowFactor, false, is_PointLight,is_SpotLight);
		
	return color;
}

vec4 CalcSpotLight(SpotLight sLight, int shadowIndex)
{
	vec3 rayDirection = normalize(FragPos - sLight.base.position);
	float slFactor = dot(rayDirection, sLight.direction);

	if(slFactor > sLight.edge)
	{
		vec4 color = CalcPointLight(sLight.base, shadowIndex, false, true);
		
		return color * (1.0f - (1.0f- slFactor)*(1.0f/(1.0f -sLight.edge)));
	}else{
		return vec4(0, 0, 0, 0);
	}
}

vec4 CalcPointLights()
{
	vec4 totalColor = vec4(0, 0, 0, 1);		//set alpha to 1 when using blending
	for(int i = 0; i < PointLightCount; i++)
	{
		totalColor += CalcPointLight(pointLights[i], i, true, false);
	}
	
	return totalColor;
}

vec4 CalcSpotLights()
{
	
	vec4 totalColor = vec4(0, 0, 0, 1); //set alpha to 1 when using blending
	for(int i = 0; i < SpotLightCount; i++)
	{
		totalColor += CalcSpotLight(spotLights[i], i + PointLightCount);
	}
	
	return totalColor;
}
	
	
void main()												
{
	albedo = texture(material.albedoMap, TexCoord).rgb;
	metallic = texture(material.metallicMap, TexCoord).r;
	roughness = texture(material.roughnessMap, TexCoord).r;
	N = normalize(Normal);
	V = normalize(FragPos-eyePosition);
	F0 = mix(F0, albedo, metallic);

	vec4 finalColor = CalcDirectionalLight();
	finalColor += CalcPointLights();
	finalColor += CalcSpotLights();
	
		 // ambient lighting (we now use IBL as the ambient term)
    vec3 kS = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    vec3 irradiance = texture(skybox, N).rgb;
    vec3 diffuse      = irradiance * albedo;
    vec3 ambient = (kD * diffuse) * texture(AOMap,CalcScreenTexCoord()).r;

	vec4 texColor = vec4(ambient,1.0)+finalColor;
	
	if(texColor.a > 1.0f)
	{
        //discard;
		texColor.a = 1.0f;
	}	

	color = texColor/*+CascadeIndicator*/;

	float brightness = dot(texColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness>1.0f)
	{
		BrightColor = vec4(texColor.rgb,1.0f);
	}
	else
	{
		BrightColor = vec4(0.0, 0.0,0.0, 1.0);
	}
	
	//motion vector
	vec3 NDCPos = (ClipSpacePos.xyz / ClipSpacePos.w)* 0.5 + 0.5;
    vec3 PrevNDCPos = (PrevClipSpacePos.xyz / PrevClipSpacePos.w)* 0.5 + 0.5;
    MotionVector = vec4(NDCPos - PrevNDCPos, 1.0f);
};