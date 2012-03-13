
//==============================================================================

#pragma once
#if !defined (__INC_LIGHTS_H_)
#define	__INC_LIGHTS_H_

//==============================================================================

#include <gml/gml.h>
#include <shadowmap.h>

//==============================================================================

enum LightType {
	LT_DIRECTIONAL,
	LT_POINT,
	LT_SPOT
};

//==============================================================================

struct BaseLight
{
	gml::vec3_t Radiance;
	gml::vec3_t AmbientRadiance;
	float AmbientIntensity;
	float DiffuseIntensity;

	BaseLight()
		: Radiance(0.6f,0.6f,0.6f)
		, AmbientRadiance(0.025f, 0.025f, 0.025f)
		, AmbientIntensity(0.0f)
		, DiffuseIntensity(0.0f)
	{
	}
};

//==============================================================================

struct DirectionalLight : public BaseLight
{        
	gml::vec3_t Direction;
	ShadowMap m_shadowmap;

	DirectionalLight()
		: m_shadowmap(LT_DIRECTIONAL)
		, Direction(0.0f, 0.0f, 0.0f)
	{
	}
};

//==============================================================================

struct PointLight : public BaseLight
{
	gml::vec3_t Position;
	ShadowMap m_shadowmap;

	struct
	{
		float Constant;
		float Linear;
		float Exp;
	} Attenuation;

	PointLight()
		: m_shadowmap(LT_POINT)
		, Position(0.0f, 0.0f, 0.0f)
	{
		Attenuation.Constant = 0.0f;
		Attenuation.Linear = 0.0f;
		Attenuation.Exp = 0.0f;
	}
};

//==============================================================================

struct SpotLight : public PointLight
{
	gml::vec3_t Direction;
	float Cutoff;

	SpotLight()
		: Direction(0.0f, 0.0f, 0.0f)
		, Cutoff(0.0f)
	{
	}
};

//==============================================================================

#endif // __INC_LIGHTS_H_

//==============================================================================

