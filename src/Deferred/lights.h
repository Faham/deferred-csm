
#ifndef LIGHTS_H
#define	LIGHTS_H

#include "../GML/gml.h"

struct BaseLight
{
	gml::vec3_t Color;
	float AmbientIntensity;
	float DiffuseIntensity;

	BaseLight()
	{
		Color = gml::vec3_t(0.0f, 0.0f, 0.0f);
		AmbientIntensity = 0.0f;
		DiffuseIntensity = 0.0f;
	}
};

struct DirectionalLight : public BaseLight
{        
	gml::vec3_t Direction;

	DirectionalLight()
	{
		Direction = gml::vec3_t(0.0f, 0.0f, 0.0f);
	}
};

struct PointLight : public BaseLight
{
	gml::vec3_t Position;

	struct
	{
		float Constant;
		float Linear;
		float Exp;
	} Attenuation;

	PointLight()
	{
		Position = gml::vec3_t(0.0f, 0.0f, 0.0f);
		Attenuation.Constant = 0.0f;
		Attenuation.Linear = 0.0f;
		Attenuation.Exp = 0.0f;
	}
};

struct SpotLight : public PointLight
{
	gml::vec3_t Direction;
	float Cutoff;

	SpotLight()
	{
		Direction = gml::vec3_t(0.0f, 0.0f, 0.0f);
		Cutoff = 0.0f;
	}
};

#define COLOR_WHITE gml::vec3_t(1.0f, 1.0f, 1.0f)
#define COLOR_RED gml::vec3_t(1.0f, 0.0f, 0.0f)
#define COLOR_GREEN gml::vec3_t(0.0f, 1.0f, 0.0f)
#define COLOR_CYAN gml::vec3_t(0.0f, 1.0f, 1.0f)
#define COLOR_BLUE gml::vec3_t(0.0f, 0.0f, 1.0f)

#endif
