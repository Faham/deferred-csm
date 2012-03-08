
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
#include <math.h>

#include "shadowmap.h"
#include "../GL3/gl3.h"
#include "../GL3/gl3w.h"
#include "../glUtils.h"
#include "../Shaders/manager.h"


// Constants for shadow map
static const int SHADOWMAP_POS_X = 0;
static const int SHADOWMAP_POS_Y = 1;
static const int SHADOWMAP_POS_Z = 2;
static const int SHADOWMAP_NEG_X = 3;
static const int SHADOWMAP_NEG_Y = 4;
static const int SHADOWMAP_NEG_Z = 5;

ShadowMap::ShadowMap()
{
	m_fbo = 0;  // Framebuffer object handle
	m_shadowmap = 0; // Cube map texture handle for shadowmap

	// TODO!
	//  -- Set up cameras (read comments below)

	// To create a shadowmap for an omni-directional point light we need cameras
	// Each camera should be:
	//   90 degree field of view
	//   1.0 aspect ratio
	//   near plane = SHADOWMAP_NEAR
	//   far plane = SHADOWMAP_FAR

	//  Furthermore, each camera needs to look down an axis as follows:
	//    camera   |    location    |    target    |     up
	//     POS_X   |     (0,0,0)    |   (1,0,0)    |   (0,-1,0)
	//     NEG_X   |     (0,0,0)    |   (-1,0,0)   |   (0,-1,0)
	//     POS_Y   |     (0,0,0)    |   (0,1,0)    |   (0,0,1)
	//     NEG_Y   |     (0,0,0)    |   (0,-1,0)   |   (0,0,-1)
	//     POS_Z   |     (0,0,0)    |   (0,0,1)    |   (0,-1,0)
	//     NEG_Z   |     (0,0,0)    |   (0,0,-1)   |   (0,-1,0)

	//   Deviation from these directions & up directions will result in an incorrect
	//  CubeMap construction.
	
	float _fov = M_PI * 0.5f;
	float _ar = 1.0f;

	gml::vec3_t _locations[6] = {	gml::vec3_t(0,0,0)
									, gml::vec3_t(0,0,0)
									, gml::vec3_t(0,0,0)
									, gml::vec3_t(0,0,0)
									, gml::vec3_t(0,0,0)
									, gml::vec3_t(0,0,0) };

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
		m_cameras[i].lookAt(_locations[i], _targets[i], _ups[i]);
		m_cameras[i].setFOV(_fov);
		m_cameras[i].setAspect(_ar);
		m_cameras[i].setDepthClip(SHADOWMAP_NEAR, SHADOWMAP_FAR);
	}
}

ShadowMap::~ShadowMap()
{
	if (m_fbo > 0) glDeleteFramebuffers(1, &m_fbo);
	if (m_shadowmap > 0) glDeleteTextures(1, &m_shadowmap);

}

// Initialize the shadowmap
//  smapSize = width & height of shadow map textures; in pixels.
// Return true if successful.
bool ShadowMap::init(const int smapSize, const Shader::Manager *manager)
{
	// TODO!

	// Step 1)
	//   Create a framebuffer object for shadow mapping, store handle # in m_fbo
	//   See: glGenFramebuffers()
	glGenFramebuffers(1, &m_fbo);

	// Step 2)
	//   Create the cube map texture for the shadow map.
	//   Store the handle for the texture in m_shadowmap

	//   a) Allocate texture object (see: glGenTextures() )
	//   b) Bind the texture handle to a GL_TEXTURE_CUBE_MAP (see: glBindTexture() )
	//   c) Set attributes/parameters of the texture as follows (see: glTexParameteri() )
	//       GL_TEXTURE_MAG_FILTER to GL_NEAREST
	//       GL_TEXTURE_MIN_FILTER to GL_NEAREST
	//       GL_TEXTURE_WRAP_{S,T,R} to GL_CLAMP_TO_EDGE
	//       GL_TEXTURE_COMPARE_MODE to GL_COMPARE_REF_TO_TEXTURE
	//       GL_TEXTURE_COMPARE_FUNC to GL_LEQUAL
	//   d) Create the 6 textures for the sides of the texture. (see: glTexImage2D() )
	//        target is one of the cube map face targets
	//        level is 0
	//        internalformat is GL_DEPTH_COMPONENT
	//        width & height are 'smapSize'
	//        border is 0
	//        format is GL_DEPTH_COMPONENT
	//        type is GL_FLOAT
	//        data is null

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
	return !isGLError();
}

void ShadowMap::create(const Object::Object **scene, const GLuint nSceneObjects,
		const gml::vec4_t &lightPos, const Camera &mainCamera)
{

	if ( !m_isReady )
	{
#if !defined(NDEBUG)
		fprintf(stderr, "Trying to create shadow map when cubemap not ready\n");
#endif
		return;
	}

	// TODO!
	//  -- Write this function that creates the cube map shadowmap

	// Step 1)
	//   Transform the given light position from world to camera coordinates
	//     camera coordinates w.r.t. the 'mainCamera'

	gml::vec3_t _light_pos = gml::extract3(gml::mul(mainCamera.getWorldView(), lightPos));
	

	// Step 2) Setup
	//   a) Bind the shadow map's FBO to the GL_DRAW_FRAMEBUFFER (see: glBindFramebuffer() )
	//   b) Set the viewport size to N x N, where N = width/height of a shadowmap
	//       (see: glViewport() )
	//   c) Turn on front-face culling (see: glEnable() and glCullFace() )
	//   d) Turn on depth-testing (see: glEnable() )
	//   e) Set the position of each m_camera[] to the camera-coord light position
	//   f) Fetch a ptr to the "Depth" shader from m_manager
	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, m_shadowMapSize, m_shadowMapSize);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);

	if (isGLError()) return;
	
	for (unsigned short i = 0; i < 6; ++i)
		m_cameras[i].setPosition(_light_pos);
		
	const Shader::Shader* _pdptshdr = m_manager->getDepthShader();

	// Step 3) Rasterize a side of the cube map [You will do this 6 times -- one for each cube side]
	//   a) Bind the texture for a cube side to the framebuffer (see: glFramebufferTexture2D() )
	//   b) Ensure the framebuffer is 'complete' (see: glCheckFramebuffer() )
	//   c) Clear the depth buffer (see: glClear() )
	//   d) Rasterize each object in the scene using the depth shader.
	//     Note: You will have to setup the shader's projection & modelview matrix
	//            The modelview matrix should transform from object -> main camera coords -> light-space coords
	//              (The m_camera[] cameras are set up to map from main camera coords to a light-space coord)
	//     Note2: See assign1::repaint() or assign2::rasterizeScene() for an idea of how to do this,
	//           but keep in mind that those rasterization loops are doing full-shading passes. Not just
	//           depth passes.
	//   e) Evoke glFinish() to wait for the GL commands to complete.
	
	for (unsigned short i = 0; i < 6; ++i) {
	
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_shadowmap, 0);
		if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)) return;
		glClear(GL_DEPTH_BUFFER_BIT);
		if (isGLError()) return;

		if (_pdptshdr->getIsReady()) {
			_pdptshdr->bindGL();
			if (isGLError()) return;

			Shader::GLProgUniforms shaderUniforms;
			shaderUniforms.m_projection = m_cameras[i].getProjection();

			for (GLuint j = 0; j < nSceneObjects; ++j)
			{
					shaderUniforms.m_modelView = gml::mul(m_cameras[i].getWorldView(), 
						gml::mul(mainCamera.getWorldView(), scene[j]->getObjectToWorld()));
					shaderUniforms.m_normalTrans = gml::transpose( gml::inverse(shaderUniforms.m_modelView) );

					if ( !_pdptshdr->setUniforms(shaderUniforms, false) || isGLError() ) return;

					scene[j]->rasterize();
					if (isGLError()) return;
			}
		
			_pdptshdr->unbindGL();
			glFinish();
		}
	
	}

	// Step 4) Tear down
	//   a) Disable face-culling (see: glDisable() )
	//   b) Disable depth-tests (see: glDisable() )

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
}


void ShadowMap::bindGL(GLenum textureUnit) const
{
	if (m_isReady)
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowmap);
	}
}

void ShadowMap::unbindGL(GLenum textureUnit) const
{
	if (m_isReady)
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}
