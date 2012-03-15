
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

#pragma once
#ifndef __INC_ROOT_H_
#define __INC_ROOT_H_

//==============================================================================

#include <config.h>
#include <gml/gml.h>
#include <camera.h>
#include <objects/object.h>
#include <objects/geometry.h>
#include <shaders/manager.h>
#include <texture/texture.h>
#include <shadowmap.h>
#include <ui.h>

#if defined (PIPELINE_DEFERRED)
#include <gbuffer.h>
class Light;
#endif

//==============================================================================

class Root : public UI::Callbacks
{
public:
	typedef std::vector<Object::Object*> ObjectVec;
	typedef std::vector<Object::Geometry*> GeometryVec;

protected:
	Shader::Manager m_shaderManager;
	Camera m_camera;
	Texture::Texture *m_texture;

	ObjectVec m_scene;

	GeometryVec m_geometries;

	unsigned int m_width;
	unsigned int m_height;

	int m_cameraMovement;
	double m_lastCamMoveTime;

	float m_movementSpeed;
	float m_rotationSpeed;
	bool m_sRGBframebuffer;
	bool m_renderWireframe;
	bool m_enableShadows;
	unsigned int m_shadowmapSize;
	// For animation
	double m_lastIdleTime; // Time that idle was last called
	void toggleCameraMoveDirection(bool enable, int direction);

#if defined (PIPELINE_DEFERRED)
	bool initLights();
	void rasterizeSceneDeferred();
	void DSGeometryPass();
#if defined (PIPELINE_DEFERRED_DEBUG)
	void DSLightPass();
#endif
	void BeginLightPasses();
	void DSPointLightsPass();
	void DSDirectionalLightPass();

	GBuffer m_gbuffer;
	bool m_gbuffer_inited;
	Object::Object * m_dummySphere;
	Object::Object * m_dummyQuad;
		
	typedef std::vector<Light*> LightVec;
	LightVec m_lights;

#else
	void rasterizeScene();
#endif

public:
	Root(unsigned int w, unsigned int h);
	virtual ~Root();

	bool init();

	virtual void windowResize(int width, int height);
	virtual void specialKeyboard(UI::KeySpecial_t key, UI::ButtonState_t state);
	virtual void repaint();
	virtual void idle();
};

//==============================================================================

#endif

//==============================================================================

