
//==============================================================================

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

#pragma once
#ifndef __INC_SHADOWMAP_H_
#define __INC_SHADOWMAP_H_

//==============================================================================

#include <gl3/gl3.h>
#include <camera.h>
#include <shaders/manager.h>
#include <objects/object.h>
#include <lights.h>
#include <vector>

//==============================================================================

#define SHADOWMAP_NEAR 1.0f
#define SHADOWMAP_NEAR_STR "1.0f"
#define SHADOWMAP_FAR 50.0f
#define SHADOWMAP_FAR_STR "50.0f"

class ShadowMap
{
private:

	typedef std::vector<Object::Object*> ObjectVec;
	typedef std::vector<Object::Geometry*> GeometryVec;
	typedef std::vector<Camera*> CameraVec;
	
	CameraVec m_cameras;
	GLuint m_shadowmap;
	GLuint m_fbo;
	int m_shadowMapSize;
	const Shader::Manager *m_manager;
	bool m_isReady;
	LightType m_type;
	float m_near;
	float m_far;

	void setupCamera(const gml::vec3_t & position = gml::vec3_t(0, 0, 0)
		, const gml::vec3_t & target = gml::vec3_t(0, 0, -1)
		, const gml::vec3_t & up = gml::vec3_t(0, 1, 0));

public:
	ShadowMap(LightType lt, const gml::vec3_t & position = gml::vec3_t(0, 0, 0)
		, const gml::vec3_t & target = gml::vec3_t(0, 0, -1)
		, const gml::vec3_t & up = gml::vec3_t(0, 1, 0));
	~ShadowMap();

	bool init(const unsigned int & smapSize, const Shader::Manager *manager);

	void create(const ObjectVec & scene, const gml::mat4x4_t &worldview
				, const gml::vec3_t & position = gml::vec3_t(0, 0, 0)
				, const gml::vec3_t & target = gml::vec3_t(0, 0, -1)
				, const gml::vec3_t & up = gml::vec3_t(0, 1, 0));

	void bindGL(GLenum textureUnit) const;
	void unbindGL(GLenum textureUnit) const;
	bool isReady() const { return m_isReady; }
	void setNear(const float & n) { m_near = n; }
	void setFar(const float & f) { m_far = f; }
	void setType(LightType lt) { m_type = lt; setupCamera(); }
	LightType getType() { return m_type; }
};

//==============================================================================

#endif // __INC_SHADOWMAP_H_

//==============================================================================

