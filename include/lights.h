
//==============================================================================

#pragma once
#if !defined (__INC_LIGHTS_H_)
#define	__INC_LIGHTS_H_

//==============================================================================

#include <gml/gml.h>
#include <ui.h>
#include <vector>
#include <shaders/manager.h>
#include <camera.h>

//==============================================================================

class ShadowMap;

namespace Object 
{
	class Object; 
	class Geometry;
}

enum LightType {
	LT_NONE,
	LT_DIRECTIONAL,
	LT_POINT,
	LT_SPOT
};

//==============================================================================

class Light
{
public:
	gml::vec3_t Radiance;
	gml::vec3_t Position;
	gml::vec3_t Direction;
	gml::vec3_t AmbientRadiance;
	LightType Type;
	float AmbientIntensity;
	float DiffuseIntensity;
	float Cutoff;
	float ConstantAttenuation;
	float LinearAttenuation;
	float ExpAttenuation;
	bool Shadow;

private:
	ShadowMap* mp_shadowmap;

public:
	typedef std::vector<Object::Object*> ObjectVec;
	typedef std::vector<Object::Geometry*> GeometryVec;

	Light();
	bool initShadow(const unsigned int & shadow_size, Shader::Manager * shader_manager);	
	void createShadow(const ObjectVec & scene, const Camera &mainCamera);
	void bindShadow(GLenum textureUnit);
};

//==============================================================================

#endif // __INC_LIGHTS_H_

//==============================================================================

