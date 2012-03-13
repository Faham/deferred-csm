
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

class ShadowMap
{
private:

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

	void setupCamera();
		
public:
	ShadowMap(LightType lt, const gml::vec3_t & position = gml::vec3_t(0, 0, 0)
		, const gml::vec3_t & target = gml::vec3_t(0, 0, -1)
		, const gml::vec3_t & up = gml::vec3_t(0, 1, 0));
	~ShadowMap();

	bool init(const int smapSize, const Shader::Manager *manager);

	void create(const Object::Object **scene, const GLuint nSceneObjects,
				const Camera &mainCamera
				, const gml::vec3_t & position = gml::vec3_t(0, 0, 0)
				, const gml::vec3_t & target = gml::vec3_t(0, 0, -1)
				, const gml::vec3_t & up = gml::vec3_t(0, 1, 0));

	void bindGL(GLenum textureUnit) const;
	void unbindGL(GLenum textureUnit) const;
	bool isReady() const { return m_isReady; }
	void setNear(const float & n) { m_near = n; }
	void setFar(const float & f) { m_far = f; }
};

//==============================================================================

#endif // __INC_SHADOWMAP_H_

//==============================================================================

