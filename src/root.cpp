
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

#include <gl3/gl3w.h>
#include <GL/glext.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <objects/models/sphere.h>
#include <objects/models/octahedron.h>
#include <objects/models/plane.h>
#include <objects/models/quad.h>

#include <shaders/material.h>
#include <glUtils.h>
#include <root.h>
#include <colors.h>

#if defined (PIPELINE_DEFERRED)
#include <lights.h>
#endif

//==============================================================================

static const int CAMERA_FORWARD = 0x001;
static const int CAMERA_BACKWARD = 0x002;
static const int CAMERA_STRAFE_RIGHT = 0x004;
static const int CAMERA_STRAFE_LEFT = 0x008;
static const int CAMERA_UP = 0x010;
static const int CAMERA_DOWN = 0x020;
static const int CAMERA_ROTATE_RIGHT = 0x040;
static const int CAMERA_ROTATE_LEFT = 0x080;
static const int CAMERA_ROTATE_UP = 0x100;
static const int CAMERA_ROTATE_DOWN = 0x200;
static const int CAMERA_SPIN_LEFT = 0x400;
static const int CAMERA_SPIN_RIGHT = 0x800;

//==============================================================================

Root::Root(unsigned int w, unsigned int h)
	: m_width(w)
	, m_height(h)
	, m_movementSpeed(2.0f)
	, m_cameraMovement(0)
	, m_rotationSpeed((40.0f * M_PI) / 180.0f)
	, m_sRGBframebuffer(false)
	, m_renderWireframe(false)
#if defined (DO_SHADOW)
	, m_enableShadows(true)
#else
	, m_enableShadows(false)
#endif
	, m_shadowmapSize(512)
#if defined (PIPELINE_DEFERRED)
	, m_gbuffer_inited(false)
#endif
{
	m_lastIdleTime = UI::getTime();
}

//------------------------------------------------------------------------------

Root::~Root()
{
	for (ObjectVec::iterator itr = m_scene.begin(); itr != m_scene.end(); ++itr)
		delete *itr;
	m_scene.clear();

	for (GeometryVec::iterator itr = m_geometries.begin(); itr != m_geometries.end(); ++itr)
		delete *itr;
	m_geometries.clear();
}

//------------------------------------------------------------------------------

bool Root::init()
{
	if ( !m_shaderManager.init() )
	{
		fprintf(stderr, "ERROR! Could not initialize Shader Manager.\n");
		return false;
	}

	m_texture = new Texture::Texture("./media/testPattern.png");
	if ( !m_texture->getIsReady() )
	{
		fprintf(stderr, "ERROR! Texture not ready\n");
		return false;
	}

	m_camera.lookAt(gml::vec3_t(5.0,0.0,5.0), gml::vec3_t(0.0,0.0,0.0) );
	m_camera.setDepthClip(1.0f, 50.0f);

	// Create the geometry.
	const int SPHERE_LOC = 0;
	const int OCTAHEDRON_LOC = 1;
	const int PLANE_LOC = 2;
	const int QUAD_LOC = 3;

	m_geometries.push_back(new Object::Models::Sphere());
	if ( !m_geometries[SPHERE_LOC] || !((Object::Models::Sphere*)m_geometries[SPHERE_LOC])->init(3) || isGLError() )
		return false;

	m_geometries.push_back(new Object::Models::Octahedron());
	if ( !m_geometries[OCTAHEDRON_LOC] || !((Object::Models::Octahedron*)m_geometries[OCTAHEDRON_LOC])->init() || isGLError())
		return false;

	m_geometries.push_back(new Object::Models::Plane());
	if ( !m_geometries[PLANE_LOC] || !((Object::Models::Plane*)m_geometries[PLANE_LOC])->init() || isGLError())
		return false;

	m_geometries.push_back(new Object::Models::Quad());
	if ( !m_geometries[QUAD_LOC] || !((Object::Models::Quad*)m_geometries[QUAD_LOC])->init() || isGLError())
		return false;

	Material::Material mat;

	const float pi2 = (90.0f * M_PI) / 180.0f;
	const float sz = 15.0f;

	mat.setTexture(m_texture);
	mat.setShaderType(Material::PHONG);
	mat.setSpecExp(16.0f);
	mat.setSurfReflectance(gml::vec3_t(0.4, 0.5, 0.6));

	mat.setSpecExp(-1.0f); // turn off specular

	mat.setSurfReflectance(Color::BEIGE);
	m_scene.push_back(new Object::Object(m_geometries[PLANE_LOC], mat, gml::mul(gml::translate(gml::vec3_t(0.0,-sz/2,0.0)), gml::scaleh(sz, 1.0, sz))));
	m_scene.push_back(new Object::Object(m_geometries[PLANE_LOC], mat, gml::mul(gml::translate(gml::vec3_t(0.0,sz/2,0.0)), gml::mul(gml::rotateZh(2*pi2),gml::scaleh(sz, 1.0, sz)))));
	mat.setSurfReflectance(Color::GREEN);
	m_scene.push_back(new Object::Object(m_geometries[PLANE_LOC], mat, gml::mul(gml::translate(gml::vec3_t(sz/2,0.0,0.0)), gml::mul(gml::rotateZh(pi2),gml::scaleh(sz, 1.0, sz)))));
	m_scene.push_back(new Object::Object(m_geometries[PLANE_LOC], mat, gml::mul(gml::translate(gml::vec3_t(-sz/2,0.0,0.0)), gml::mul(gml::rotateZh(-pi2),gml::scaleh(sz, 1.0, sz)))));
	mat.setSurfReflectance(Color::RED);
	m_scene.push_back(new Object::Object(m_geometries[PLANE_LOC], mat, gml::mul(gml::translate(gml::vec3_t(0.0,0.0,sz/2)), gml::mul(gml::rotateXh(-pi2),gml::scaleh(sz, 1.0, sz)))));
	m_scene.push_back(new Object::Object(m_geometries[PLANE_LOC], mat, gml::mul(gml::translate(gml::vec3_t(0.0,0.0,-sz/2)), gml::mul(gml::rotateXh(pi2),gml::scaleh(sz, 1.0, sz)))));

	m_scene.push_back(new Object::Object(m_geometries[SPHERE_LOC], mat, gml::mul(gml::translate(gml::vec3_t(0.0,0.0,0.0)), gml::scaleh(2.5,0.75,1.5))));
	m_scene.push_back(new Object::Object(m_geometries[OCTAHEDRON_LOC], mat, gml::mul(gml::translate(gml::vec3_t(0.0,1.5,0.0)), gml::scaleh(1.0,1.5,1.0))));

#if defined (PIPELINE_DEFERRED)
	m_dummySphere = new Object::Object(m_geometries[SPHERE_LOC], mat, gml::identity4());
	m_dummyQuad = new Object::Object(m_geometries[QUAD_LOC], mat, gml::identity4());
	if (!initLights())
		return false;
#endif

	printf(
			"Camera movement:\n"
			"  [w] -- Camera forward\n"
			"  [s] -- Camera backward\n"
			"  [q] -- Camera up\n"
			"  [e] -- Camera down\n"
			"  [a] -- Camera strafe left\n"
			"  [d] -- Camera strafe right\n"
			"Camera rotation:\n"
			"  [keypad 8] -- Rotate camera up\n"
			"  [keypad 5] -- Rotate camera down\n"
			"  [keypad 4] -- Rotate camera right\n"
			"  [keypad 6] -- Rotate camera left\n"
			"  [keypad 7] -- Spin camera left\n"
			"  [keypad 9] -- Spin camera right\n"
			"Other Controls:\n"
			"  [F1] -- Toggle shadows\n"
			"  [g] -- Toggle sRGB framebuffer\n"
			"  [f] -- Toggle wireframe rendering\n"
			"  [o] -- Set to orthographic camera\n"
			"  [p] -- Set to perspective camera\n"
			"  [ESC] -- Quit\n"
	);

	return true;
}

//------------------------------------------------------------------------------

void Root::windowResize(int width, int height)
{
	float aspect = width / (float)height;
	m_camera.setAspect(aspect);

	m_width = width;
	m_height = height;
}

//------------------------------------------------------------------------------

void Root::toggleCameraMoveDirection(bool enable, int direction)
{
	if (enable) // enable movement
		m_cameraMovement = m_cameraMovement | direction;
	else
		m_cameraMovement = m_cameraMovement & (~direction);
	m_lastCamMoveTime = UI::getTime();
}

//------------------------------------------------------------------------------

void Root::specialKeyboard(UI::KeySpecial_t key, UI::ButtonState_t state)
{
	/*
	 * Called automatically when the user presses a key on the keyboard
	 * key will be a code indicating which key was pressed
	 * state will be one of UI::BUTTON_UP or UI::BUTTON_DOWN
	 *   indicating whether the key was released, or pressed
	 */
	switch (key)
	{
	case UI::KEY_ESC:
		if (state == UI::BUTTON_UP)
		{
			UI::exitMainLoop();
		}
		break;
	case UI::KEY_W:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_FORWARD);
		break;
	case UI::KEY_S:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_BACKWARD);
		break;

	case UI::KEY_A:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_STRAFE_LEFT);
		break;
	case UI::KEY_D:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_STRAFE_RIGHT);
		break;

	case UI::KEY_Q:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_UP);
		break;
	case UI::KEY_E:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_DOWN);
		break;


	case UI::KEY_KP_8:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_ROTATE_UP);
		break;
	case UI::KEY_KP_5:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_ROTATE_DOWN);
		break;

	case UI::KEY_KP_4:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_ROTATE_LEFT);
		break;
	case UI::KEY_KP_6:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_ROTATE_RIGHT);
		break;

	case UI::KEY_KP_7:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_SPIN_LEFT);
		break;
	case UI::KEY_KP_9:
		toggleCameraMoveDirection(state == UI::BUTTON_DOWN, CAMERA_SPIN_RIGHT);
		break;

	case UI::KEY_F1:
		if (state == UI::BUTTON_DOWN)
			m_enableShadows = !m_enableShadows;
		break;

	case UI::KEY_G:
		if (state == UI::BUTTON_DOWN)
		{
			m_sRGBframebuffer = !m_sRGBframebuffer;
			if (m_sRGBframebuffer)
			{
				printf("sRGB framebuffer enabled\n");
			}
			else
			{
				printf("sRGB framebuffer disabled\n");
			}
		}
		break;
	case UI::KEY_F:
		if (state == UI::BUTTON_DOWN)
		{
			m_renderWireframe = !m_renderWireframe;
		}
		break;

	case UI::KEY_O:
		m_camera.setCameraProjection(CAMERA_PROJECTION_ORTHOGRAPHIC);
		break;

	case UI::KEY_P:
		m_camera.setCameraProjection(CAMERA_PROJECTION_PERSPECTIVE);
		break;

	default:
		break;
	}
}

//------------------------------------------------------------------------------

#if defined (PIPELINE_DEFERRED)

bool Root::initLights()
{
	Light * l1 = new Light();
    l1->Type = LT_SPOT;
    l1->AmbientIntensity = 0.0f;
    l1->DiffuseIntensity = 0.9f;
	l1->Radiance = Color::WHITE;
    l1->LinearAttenuation = 0.01f;
    l1->Position  = gml::vec3_t(-20.0, 20.0, 5.0f);
    l1->Direction = gml::vec3_t(1.0f, -1.0f, 0.0f);
    l1->Cutoff =  20.0f;
	m_lights.push_back(l1);

	Light * l2 = new Light();
    l2->Type = LT_DIRECTIONAL;
	l2->AmbientIntensity = 0.1f;
	l2->Radiance = Color::WHITE;
	l2->DiffuseIntensity = 0.5f;
	l2->Direction = gml::vec3_t(1.0f, 0.0f, 0.0f);
	m_lights.push_back(l2);

	Light * l3 = new Light();
    l3->Type = LT_POINT;
	l3->DiffuseIntensity = 0.2f;
	l3->Radiance = Color::GREEN;
    l3->Position = gml::vec3_t(0.0f, 1.5f, 5.0f);
	l3->ConstantAttenuation = 0.0f;
    l3->LinearAttenuation = 0.0f;
    l3->ExpAttenuation = 1.0f;
	m_lights.push_back(l3);

	Light * l4 = new Light();
    l4->Type = LT_POINT;
	l4->DiffuseIntensity = 0.2f;
	l4->Radiance = Color::RED;
    l4->Position = gml::vec3_t(2.0f, 0.0f, 5.0f);
	l4->ConstantAttenuation = 0.0f;
    l4->LinearAttenuation = 0.0f;
    l4->ExpAttenuation = 1.0f;
	m_lights.push_back(l4);
    
	Light * l5 = new Light();
    l5->Type = LT_POINT;
	l5->DiffuseIntensity = 0.2f;
	l5->Radiance = Color::BLUE;
    l5->Position = gml::vec3_t(0.0f, 0.0f, 3.0f);
	l5->ConstantAttenuation = 0.0f;
    l5->LinearAttenuation = 0.0f;        
    l5->ExpAttenuation = 1.0f;
#if defined (DO_SHADOW)
    l5->Shadow = true;
    l5->initShadow(m_shadowmapSize, &m_shaderManager);
#endif
	m_lights.push_back(l5);
/*
#if defined (DO_SHADOW)
	for (LightVec::iterator itr = m_lights.begin(); itr != m_lights.end(); ++itr)
		if (!(*itr)->initShadow(m_shadowmapSize, &m_shaderManager))
		{
			fprintf(stderr, "Failed to initialize shadow mapping members.\n");
			return false;
		}
#endif
*/
	return true;
}

//------------------------------------------------------------------------------

void Root::rasterizeSceneDeferred()
{
	if (m_sRGBframebuffer)
		glEnable(GL_FRAMEBUFFER_SRGB_EXT);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDepthMask(GL_TRUE); // Only the Geometry Pass update the depth buffer
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (isGLError()) return;

	Shader::GLProgUniforms shaderUniforms;
	shaderUniforms.m_projection = m_camera.getProjection();

	const Shader::Shader *shader = m_shaderManager.getDeferredGeometryPassShader();

	if (shader->getIsReady(false))
	{
		shader->bindGL(false);
		if (isGLError()) return;

		for (ObjectVec::iterator itr = m_scene.begin(); itr != m_scene.end(); ++itr)
		{
			shaderUniforms.m_modelView = gml::mul(m_camera.getWorldView(), (*itr)->getObjectToWorld());
			shaderUniforms.m_normalTrans = gml::transpose(gml::inverse(shaderUniforms.m_modelView));
			(*itr)->getMaterial().getTexture()->bindGL(GL_TEXTURE0);

			if ( !shader->setUniforms(shaderUniforms, m_enableShadows) || isGLError() ) return;

			(*itr)->rasterize();
			if (isGLError()) return;
		}
		shader->unbindGL();
	}

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST); // No point in depth testing in light pass
    glDepthMask(GL_FALSE);

	if (m_sRGBframebuffer)
		glDisable(GL_FRAMEBUFFER_SRGB_EXT);

	if (m_renderWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glFinish();
}

//------------------------------------------------------------------------------

void Root::DSGeometryPass()
{
    m_gbuffer.BindForWriting();
	glViewport(0,0,m_width,m_height);
	rasterizeSceneDeferred();
}

//------------------------------------------------------------------------------

void Root::BeginLightPasses()
{
	// this is for accumulating each lighting pass
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	m_gbuffer.BindForReading();
	glClear(GL_COLOR_BUFFER_BIT);
}

//------------------------------------------------------------------------------

float CalcPointLightBSphere(const gml::vec3_t& Color, float Intensity)
{
   	float MaxChannel = fmax(fmax(Color.x, Color.y), Color.z);
   	float c = MaxChannel * Intensity;
   	return (8.0f * sqrtf(c) + 1.0f);
}

//------------------------------------------------------------------------------

void Root::DSPointLightsPass()
{
	Shader::GLProgUniforms shaderUniforms;
	shaderUniforms.m_projection = m_camera.getProjection();
	shaderUniforms.m_ds_ScreenSize = gml::vec2_t(m_width, m_height);

	const Shader::Shader *shader = m_shaderManager.getDeferredPointLightPassShader();

	if (shader->getIsReady(false))
	{
		shader->bindGL(false);
		if (isGLError()) return;

		for (LightVec::iterator itr = m_lights.begin(); itr < m_lights.end(); ++itr)
		{
			Light& lit = **itr;
			if (lit.Type != LT_POINT)
				continue;
			lit.bindShadow(GL_TEXTURE3);
			if (isGLError()) return;
			shaderUniforms.m_lightPos = gml::extract3(gml::mul(m_camera.getWorldView(), gml::vec4_t(lit.Position, 1.0f)));
			shaderUniforms.m_lightRad = lit.Radiance;
			shaderUniforms.m_ds_AmbientIntensity = lit.AmbientIntensity;
			shaderUniforms.m_ds_DiffuseIntensity = lit.DiffuseIntensity;
			shaderUniforms.m_ds_AttenuationConstant = lit.ConstantAttenuation;
			shaderUniforms.m_ds_AttenuationLinear = lit.LinearAttenuation;
			shaderUniforms.m_ds_AttenuationExp = lit.ExpAttenuation;

			float _scale = CalcPointLightBSphere(lit.Radiance, lit.DiffuseIntensity);
			shaderUniforms.m_modelView = gml::mul(m_camera.getWorldView(), gml::mul(gml::translate(lit.Position), gml::scaleh(_scale, _scale, _scale)));

			if ( !shader->setUniforms(shaderUniforms, m_enableShadows) || isGLError() ) return;

			m_dummySphere->rasterize();
			if (isGLError()) return;
		}
		shader->unbindGL();
	}
	
	glFinish();
}

//------------------------------------------------------------------------------

void Root::DSDirectionalLightPass()
{
	Shader::GLProgUniforms shaderUniforms;
	shaderUniforms.m_projection = m_camera.getProjection();
	shaderUniforms.m_ds_ScreenSize = gml::vec2_t(m_width, m_height);
	shaderUniforms.m_modelView = gml::identity4();

	const Shader::Shader *shader = m_shaderManager.getDeferredDirectionalLightPassShader();

	if (shader->getIsReady(false))
	{
		shader->bindGL(false);
		if (isGLError()) return;

		for (LightVec::iterator itr = m_lights.begin(); itr < m_lights.end(); ++itr)
		{
			Light& lit = **itr;
			if (lit.Type != LT_DIRECTIONAL)
				continue;
			lit.bindShadow(GL_TEXTURE3);
			if (isGLError()) return;
			shaderUniforms.m_lightRad = lit.Radiance;
			shaderUniforms.m_ds_AmbientIntensity = lit.AmbientIntensity;
			shaderUniforms.m_ds_DiffuseIntensity = lit.DiffuseIntensity;
			shaderUniforms.m_ds_DirectionalLightDirection = gml::extract3(gml::normalize(gml::mul(m_camera.getWorldView(), gml::vec4_t(lit.Direction, 1.0))));

			if ( !shader->setUniforms(shaderUniforms, m_enableShadows) || isGLError() ) return;

			m_dummyQuad->rasterize();
			if (isGLError()) return;
		}
		shader->unbindGL();
	}
	
	glFinish();
}

//------------------------------------------------------------------------------

#if defined (PIPELINE_DEFERRED_DEBUG)

void Root::DSLightPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_gbuffer.BindForReading();

	GLsizei HalfWidth = (GLsizei)(m_width / 2.0f);
	GLsizei HalfHeight = (GLsizei)(m_height / 2.0f);

	m_gbuffer.SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
	glBlitFramebuffer(0, 0, m_width, m_height,0, 0, HalfWidth, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	m_gbuffer.SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
	glBlitFramebuffer(0, 0, m_width, m_height, 0, HalfHeight, HalfWidth, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	m_gbuffer.SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
	glBlitFramebuffer(0, 0, m_width, m_height, HalfWidth, HalfHeight, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	m_gbuffer.SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_TEXCOORD);
	glBlitFramebuffer(0, 0, m_width, m_height, HalfWidth, 0, m_width, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);	
}

#endif

//------------------------------------------------------------------------------

#else

void Root::rasterizeScene()
{
	if (m_renderWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (m_sRGBframebuffer)
		glEnable(GL_FRAMEBUFFER_SRGB_EXT);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (isGLError()) return;

	if (m_enableShadows)
	{
		m_shadowmap.bindGL(GL_TEXTURE1);
		if (isGLError()) return;
	}
	
	Shader::GLProgUniforms shaderUniforms;
	shaderUniforms.m_lightPos = gml::extract3( gml::mul( m_camera.getWorldView(), m_lightPos ) );
	shaderUniforms.m_lightRad = m_lightRad;
	shaderUniforms.m_ambientRad = m_ambientRad;
	shaderUniforms.m_projection = m_camera.getProjection();

	for (GLuint i=0; i<m_nObjects; i++)
	{
		// Fetch the Shader object from the ShaderManager that will perform the
		// shading calculations for this object
		const Shader::Shader *shader = m_shaderManager.getShader(m_scene[i]->getMaterial());

		if (shader->getIsReady(m_enableShadows))
		{
			shader->bindGL(m_enableShadows); // Bind the shader to the OpenGL context
			if (isGLError()) return;

			// Object-specific uniforms
			shaderUniforms.m_modelView = gml::mul(m_camera.getWorldView(), m_scene[i]->getObjectToWorld());
			shaderUniforms.m_normalTrans = gml::transpose( gml::inverse(shaderUniforms.m_modelView) );
			// If the surface material is not using a texture for Lambertian surface reflectance
			if (m_scene[i]->getMaterial().getLambSource() == Material::CONSTANT)
			{
				shaderUniforms.m_surfRefl = m_scene[i]->getMaterial().getSurfRefl();
			}
			else
			{
				m_scene[i]->getMaterial().getTexture()->bindGL(GL_TEXTURE0); // Set up texture
			}
			// Set up the specular components of the uniforms struct if the material
			// is specular
			if (m_scene[i]->getMaterial().hasSpecular())
			{
				shaderUniforms.m_specExp = m_scene[i]->getMaterial().getSpecExp();
				shaderUniforms.m_specRefl = m_scene[i]->getMaterial().getSpecRefl();
			}

			// Set the shader uniform variables
			if ( !shader->setUniforms(shaderUniforms, m_enableShadows) || isGLError() ) return;

			// Rasterize the object
			m_scene[i]->rasterize();
			if (isGLError()) return;

			// Unbind the shader from the OpenGL context
			shader->unbindGL();
		}
	}

	m_shadowmap.unbindGL(GL_TEXTURE1);

	// Put the render state back the way we found it
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	if (m_sRGBframebuffer)
	{
		glDisable(GL_FRAMEBUFFER_SRGB_EXT);
	}
	if (m_renderWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glFinish();
}

#endif

//------------------------------------------------------------------------------

void Root::repaint()
{
#if defined (PIPELINE_DEFERRED)

	if (!m_gbuffer_inited) {
	 	m_gbuffer.Init(m_width, m_height);
	 	m_gbuffer_inited = true;
	 }
#if defined (DO_SHADOW)
	if (m_enableShadows)
	{
		for (LightVec::iterator itr = m_lights.begin(); itr != m_lights.end(); ++itr)
			(*itr)->createShadow(m_scene, m_camera);
		if ( isGLError() ) return;
	}
#endif

	DSGeometryPass();
#if defined (PIPELINE_DEFERRED_DEBUG)
	DSLightPass();
#else	
	BeginLightPasses();
	DSPointLightsPass();
	DSDirectionalLightPass();
#endif

#else
	if (m_enableShadows)
	{
		m_shadowmap.create((const Object::Object**)m_scene, m_nObjects, m_lightPos, m_camera);
		if ( isGLError() ) return;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glViewport(0,0,m_width,m_height);
	rasterizeScene();
#endif
}

//------------------------------------------------------------------------------

void Root::idle()
{
	// idle is called each time through the event loop when there is no
	// pending render call, or interaction event to process.
	// That makes it ideal for animation, and similar actions

	double currTime = UI::getTime(); // current time

	if (m_cameraMovement) // Is a camera movement key pressed?
	{
		// time since the last time we updated the camera
		double deltaT = currTime - m_lastCamMoveTime;
		if (deltaT > 0)
		{
			// Time has elapsed since the last time idle() was called
			// so, move the camera according to which key(s) are pressed.

			if (m_cameraMovement & CAMERA_FORWARD)
				m_camera.moveForward( m_movementSpeed * deltaT );
			if (m_cameraMovement & CAMERA_BACKWARD)
				m_camera.moveForward( -m_movementSpeed * deltaT );

			if (m_cameraMovement & CAMERA_STRAFE_RIGHT)
				m_camera.strafeRight( m_movementSpeed * deltaT );
			if (m_cameraMovement & CAMERA_STRAFE_LEFT)
				m_camera.strafeRight( -m_movementSpeed * deltaT );

			if (m_cameraMovement & CAMERA_UP)
				m_camera.moveUp( m_movementSpeed * deltaT);
			if (m_cameraMovement & CAMERA_DOWN)
				m_camera.moveUp( -m_movementSpeed * deltaT);

			if (m_cameraMovement & CAMERA_ROTATE_UP)
				m_camera.rotateUp( m_rotationSpeed * deltaT );
			if (m_cameraMovement & CAMERA_ROTATE_DOWN)
				m_camera.rotateUp( -m_rotationSpeed * deltaT );

			if (m_cameraMovement & CAMERA_ROTATE_LEFT)
				m_camera.rotateRight( m_rotationSpeed * deltaT );
			if (m_cameraMovement & CAMERA_ROTATE_RIGHT)
				m_camera.rotateRight( -m_rotationSpeed * deltaT );

			if (m_cameraMovement & CAMERA_SPIN_LEFT)
				m_camera.spinCamera( m_rotationSpeed * deltaT );
			if (m_cameraMovement & CAMERA_SPIN_RIGHT)
				m_camera.spinCamera( -m_rotationSpeed * deltaT );

			m_lastCamMoveTime = currTime;
		}
	}

	m_lastIdleTime = currTime;
}

//==============================================================================


