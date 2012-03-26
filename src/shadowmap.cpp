
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

//==============================================================================

#include <cstdio>
#include <math.h>
#include <shadowmap.h>
#include <gl3/gl3.h>
#include <gl3/gl3w.h>
#include <glUtils.h>
#include <shaders/manager.h>
#include <lights.h>

//==============================================================================

ShadowMap::	ShadowMap(LightType lt, const gml::vec3_t & position
		, const gml::vec3_t & target, const gml::vec3_t & up)
{
	m_fbo = 0;
	m_shadowmap = 0;
	m_type = lt;
	m_near = 1.0;
	m_far = 50.0;
	m_shadowMapSize = 512;
	Shader::Manager *m_manager = NULL;
	m_isReady = false;
	setupCamera(position, target, up);
}

//------------------------------------------------------------------------------

void ShadowMap::setupCamera(const gml::vec3_t & position
						, const gml::vec3_t & target, const gml::vec3_t & up)
{
	for (CameraVec::iterator itr = m_cameras.begin(); itr != m_cameras.end(); ++itr)
		delete *itr;
	m_cameras.clear();


	if (LT_POINT == m_type)
	{
		gml::vec3_t _targets[6] = {	gml::vec3_t(1,0,0)
									, gml::vec3_t(-1,0,0)
									, gml::vec3_t(0,1,0)
									, gml::vec3_t(0,-1,0)
									, gml::vec3_t(0,0,1)
									, gml::vec3_t(0,0,-1) };

		gml::vec3_t _ups[6] = {	gml::vec3_t(0,-1,0)
								, gml::vec3_t(0,-1,0)
								, gml::vec3_t(0,0,1)
								, gml::vec3_t(0,0,-1)
								, gml::vec3_t(0,-1,0)
								, gml::vec3_t(0,-1,0) };

	
		for (unsigned short i = 0; i < 6; ++i) {
			Camera* cam = new Camera();
			cam->lookAt(position, _targets[i], _ups[i]);
			cam->setFOV(M_PI * 0.5f);
			cam->setAspect(1.0f);
			cam->setDepthClip(m_near, m_far);
			m_cameras.push_back(cam);
		}
	}
	else if (LT_DIRECTIONAL == m_type)
	{
		Camera* cam = new Camera();
		
		cam->lookAt(position, target, up);
		cam->setFOV(M_PI * 0.5f);
		cam->setAspect(1.0f);
		cam->setDepthClip(m_near, m_far);
		m_cameras.push_back(cam);
	}
}

//------------------------------------------------------------------------------

ShadowMap::~ShadowMap()
{
	if (m_fbo > 0) glDeleteFramebuffers(1, &m_fbo);
	if (m_shadowmap > 0) glDeleteTextures(1, &m_shadowmap);

}

//------------------------------------------------------------------------------

bool ShadowMap::init(const unsigned int & smapSize, const Shader::Manager *manager)
{
/*/
	glGenFramebuffers(1, &m_fbo);

	glGenTextures(1, &m_shadowmap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowmap);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// GL_NEAREST, GL_LINEAR
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	// GL_NEAREST, GL_LINEAR
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	for (unsigned short i = 0; i < 6; ++i) 
		glTexImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, smapSize, smapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	m_manager = manager;
	m_shadowMapSize = smapSize;
	m_isReady = glIsTexture(m_shadowmap) == GL_TRUE;

	printf("Light:%i, Is shadowmap ready:%s\n", (int)m_type, m_isReady ? "True": "False");

	return !isGLError();
/*/
	glGenFramebuffers(1, &m_fbo);

	glGenTextures(1, &m_shadowmap);

	if (LT_POINT == m_type)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowmap);
	
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// GL_NEAREST, GL_LINEAR
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	// GL_NEAREST, GL_LINEAR
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

		for (unsigned short i = 0; i < 6; ++i) 
			glTexImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, smapSize, smapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	else if (LT_DIRECTIONAL == m_type)
	{
		glBindTexture(GL_TEXTURE_2D, m_shadowmap);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// TODO: Check two following options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

		glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, smapSize, smapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	
	m_manager = manager;
	m_shadowMapSize = smapSize;
	m_isReady = glIsTexture(m_shadowmap) == GL_TRUE;

	//printf("Is shadowmap ready:%s\n", m_isReady ? "True": "False");

	return !isGLError();
//*/
}

//------------------------------------------------------------------------------

void ShadowMap::create(const ObjectVec & scene, const gml::mat4x4_t &worldview
					, const gml::vec3_t & position, const gml::vec3_t & target
					, const gml::vec3_t & up)
{
	if ( !m_isReady )
	{
#if !defined(NDEBUG)
		fprintf(stderr, "Trying to create shadow map when cubemap not ready\n");
#endif
		return;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, m_shadowMapSize, m_shadowMapSize);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);

	if (isGLError()) return;
	
	if (LT_POINT == m_type)
	{
		gml::vec3_t _light_pos = gml::extract3(gml::mul(worldview, gml::vec4_t(position, 1.0)));
		for (unsigned short i = 0; i < 6; ++i)
			m_cameras[i]->setPosition(_light_pos);
	}
	else if (LT_DIRECTIONAL == m_type)
	{
		gml::vec3_t _light_pos = gml::extract3(gml::mul(worldview, gml::vec4_t(position, 1.0)));
		gml::vec3_t _light_target = gml::extract3(gml::mul(worldview, gml::vec4_t(target, 1.0)));
		gml::vec3_t _light_up = gml::extract3(gml::mul(worldview, gml::vec4_t(up, 1.0)));
		m_cameras[0]->lookAt(_light_pos, _light_target, _light_up);
	}
		
	const Shader::Shader* _pdptshdr = m_manager->getDepthShader();

	if (LT_POINT == m_type)
	{
		for (unsigned short i = 0; i < 6; ++i) {
	
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_shadowmap, 0);
			if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)) return;
			glClear(GL_DEPTH_BUFFER_BIT);
			if (isGLError()) return;

			if (_pdptshdr->getIsReady()) {
				_pdptshdr->bindGL();
				if (isGLError()) return;

				Shader::GLProgUniforms shaderUniforms;
				shaderUniforms.m_projection = m_cameras[i]->getProjection();

				for (ObjectVec::const_iterator itr = scene.begin(); itr != scene.end(); ++itr)
				{
					shaderUniforms.m_modelView = gml::mul(m_cameras[i]->getWorldView(), 
						gml::mul(worldview, (*itr)->getObjectToWorld()));
					shaderUniforms.m_normalTrans = gml::transpose( gml::inverse(shaderUniforms.m_modelView) );

					if ( !_pdptshdr->setUniforms(shaderUniforms, false) || isGLError() ) return;

					(*itr)->rasterize();
					if (isGLError()) return;
				}
		
				_pdptshdr->unbindGL();
				glFinish();
			}
		}
	}
	else if (LT_DIRECTIONAL == m_type)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowmap, 0);
		if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)) return;
		glClear(GL_DEPTH_BUFFER_BIT);
		if (isGLError()) return;

		if (_pdptshdr->getIsReady()) {
			_pdptshdr->bindGL();
			if (isGLError()) return;

			Shader::GLProgUniforms shaderUniforms;
			shaderUniforms.m_projection = m_cameras[0]->getProjection();

			for (ObjectVec::const_iterator itr = scene.begin(); itr != scene.end(); ++itr)
			{
				shaderUniforms.m_modelView = gml::mul(m_cameras[0]->getWorldView(), 
					gml::mul(worldview, (*itr)->getObjectToWorld()));
				shaderUniforms.m_normalTrans = gml::transpose( gml::inverse(shaderUniforms.m_modelView) );

				if ( !_pdptshdr->setUniforms(shaderUniforms, false) || isGLError() ) return;

				(*itr)->rasterize();
				if (isGLError()) return;
			}
	
			_pdptshdr->unbindGL();
			glFinish();
		}
	}
	

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
}

//------------------------------------------------------------------------------

void ShadowMap::bindGL(GLenum textureUnit) const
{
	if (m_isReady)
	{
		glActiveTexture(textureUnit);
		if (LT_POINT == m_type)
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowmap);
		else if (LT_DIRECTIONAL == m_type)
			glBindTexture(GL_TEXTURE_2D, m_shadowmap);
	}
}

//------------------------------------------------------------------------------

void ShadowMap::unbindGL(GLenum textureUnit) const
{
	if (m_isReady)
	{
		glActiveTexture(textureUnit);
		if (LT_POINT == m_type)
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		else if (LT_DIRECTIONAL == m_type)
			glBindTexture(GL_TEXTURE_2D, 0);
	}
}

//==============================================================================

