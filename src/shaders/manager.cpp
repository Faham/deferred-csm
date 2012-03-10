
/*
 * Copyright:
 * Daniel D. Neilson (ddneilson@ieee.org)
 * University of Saskatchewan
 * All rights reserved
 *
 * Permission granted to use for use in assignments and
 * projects for CMPT 485 & CMPT 829 at the University
 * of Saskatchewan.
 */

#include <cstdio>
#include <cstring>

#include <shaders/manager.h>

#include <shaders/constant/simple.h>
#include <shaders/constant/depth.h>
#include <shaders/constant/lambertian/gouraud.h>
#include <shaders/constant/lambertian/phong.h>
#include <shaders/constant/specular/gouraud.h>
#include <shaders/constant/specular/phong.h>
#include <shaders/texture/lambertian/gouraud.h>
#include <shaders/texture/lambertian/phong.h>
#include <shaders/texture/specular/gouraud.h>
#include <shaders/texture/specular/phong.h>
#include <shaders/deferred/geometrypass.h>
#include <shaders/deferred/pointlightpass.h>
#include <shaders/deferred/directionallightpass.h>

namespace Shader
{

typedef enum 
{
	SIMPLE = 0,
	DEPTH,
	CONST_LAMB_GOURAUD,
	CONST_LAMB_PHONG,
	CONST_SPEC_GOURAUD,
	CONST_SPEC_PHONG,
	TEXTURE_LAMB_GOURAUD,
	TEXTURE_LAMB_PHONG,
	TEXTURE_SPEC_GOURAUD,
	TEXTURE_SPEC_PHONG,
	DEFERRED_GEOMETRY_PASS,
	DEFERRED_POINTLIGHT_PASS,
	DEFERRED_DIRECTIONALLIGHT_PASS,
	NUM_SHADERS
} ShaderOffsets;

Manager::Manager()
{
	m_shaders = 0;
	m_nShaders = 0;
}
Manager::~Manager()
{
	if (m_shaders)
	{
		for (int i=0; i<m_nShaders; i++)
		{
			if (m_shaders[i]) delete m_shaders[i];
		}
		delete[] m_shaders;
	}
}

bool Manager::init()
{
	m_nShaders = NUM_SHADERS;
	m_shaders = new Shader*[m_nShaders];
	memset(m_shaders, 0x00, sizeof(Shader*)*m_nShaders);

	m_shaders[SIMPLE] = new Constant::Simple();
	if ( !m_shaders[SIMPLE] ) return false;

	m_shaders[DEPTH] = new Constant::Depth();
	if ( !m_shaders[DEPTH] ) return false;

	m_shaders[CONST_LAMB_GOURAUD] = new Constant::Lambertian::Gouraud();
	if ( !m_shaders[CONST_LAMB_GOURAUD] ) return false;

	m_shaders[CONST_LAMB_PHONG] = new Constant::Lambertian::Phong();
	if ( !m_shaders[CONST_LAMB_PHONG] ) return false;

	m_shaders[CONST_SPEC_GOURAUD] = new Constant::Specular::Gouraud();
	if ( !m_shaders[CONST_SPEC_GOURAUD] ) return false;

	m_shaders[CONST_SPEC_PHONG] = new Constant::Specular::Phong();
	if ( !m_shaders[CONST_SPEC_PHONG] ) return false;

	m_shaders[TEXTURE_LAMB_GOURAUD] = new Texture::Lambertian::Gouraud();
	if ( !m_shaders[TEXTURE_LAMB_GOURAUD] ) return false;

	m_shaders[TEXTURE_LAMB_PHONG] = new Texture::Lambertian::Phong();
	if ( !m_shaders[TEXTURE_LAMB_PHONG] ) return false;

	m_shaders[TEXTURE_SPEC_GOURAUD] = new Texture::Specular::Gouraud();
	if ( !m_shaders[TEXTURE_SPEC_GOURAUD] ) return false;

	m_shaders[TEXTURE_SPEC_PHONG] = new Texture::Specular::Phong();
	if ( !m_shaders[TEXTURE_SPEC_PHONG] ) return false;

	m_shaders[DEFERRED_GEOMETRY_PASS] = new Deferred::GeometryPass();
	if ( !m_shaders[DEFERRED_GEOMETRY_PASS] ) return false;

	m_shaders[DEFERRED_POINTLIGHT_PASS] = new Deferred::PointLightPass();
	if ( !m_shaders[DEFERRED_POINTLIGHT_PASS] ) return false;

	m_shaders[DEFERRED_DIRECTIONALLIGHT_PASS] = new Deferred::DirectionalLightPass();
	if ( !m_shaders[DEFERRED_DIRECTIONALLIGHT_PASS] ) return false;

	return true;
}

const Shader* Manager::getShader(const Material::Material &mat) const
{
	switch (mat.getShaderType())
	{
	case Material::GOURAUD:
		switch (mat.getLambSource())
		{
		case Material::CONSTANT:
			if (!mat.hasSpecular())
			{
				return m_shaders[CONST_LAMB_GOURAUD];
			}
			else
			{
				return m_shaders[CONST_SPEC_GOURAUD];
			}
		case Material::TEXTURE:
			if (!mat.hasSpecular())
			{
				return m_shaders[TEXTURE_LAMB_GOURAUD];
			}
			else
			{
				return m_shaders[TEXTURE_SPEC_GOURAUD];
			}
		}
		break;
	case Material::PHONG:
		switch (mat.getLambSource())
		{
		case Material::CONSTANT:
			if (!mat.hasSpecular())
			{
				return m_shaders[CONST_LAMB_PHONG];
			}
			else
			{
				return m_shaders[CONST_SPEC_PHONG];
			}
		case Material::TEXTURE:
			if (!mat.hasSpecular())
			{
				return m_shaders[TEXTURE_LAMB_PHONG];
			}
			else
			{
				return m_shaders[TEXTURE_SPEC_PHONG];
			}
		}
		break;
	default:
		return m_shaders[SIMPLE];
	}
	return m_shaders[SIMPLE];
}

const Shader* Manager::getDepthShader() const
{
	return m_shaders[DEPTH];
}

const Shader* Manager::getDeferredGeometryPassShader() const
{
	return m_shaders[DEFERRED_GEOMETRY_PASS];
}

const Shader* Manager::getDeferredPointLightPassShader() const
{
	return m_shaders[DEFERRED_POINTLIGHT_PASS];
}

const Shader* Manager::getDeferredDirectionalLightPassShader() const
{
	return m_shaders[DEFERRED_DIRECTIONALLIGHT_PASS];
}

}
