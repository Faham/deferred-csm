
//==============================================================================

#include <lights.h>
#include <shadowmap.h>
#include <objects/object.h>
#include <objects/geometry.h>

//==============================================================================

Light::Light()
	: Radiance(0.6f,0.6f,0.6f)
	, Position(0.0f, 0.0f, 0.0f)
	, Direction(0.0f, 0.0f, 0.0f)
	, AmbientRadiance(0.025f, 0.025f, 0.025f)
	, AmbientIntensity(0.0f)
	, DiffuseIntensity(0.0f)
	, Cutoff(0.0f)
	, ConstantAttenuation(0.0f)
	, LinearAttenuation(0.0f)
	, ExpAttenuation(0.0f)
	, Shadow(false)
	, m_type(LT_NONE)
{
	mp_shadowmap = new ShadowMap(m_type);
}

//------------------------------------------------------------------------------

bool Light::initShadow(const unsigned int & shadow_size, Shader::Manager * shader_manager)
{
	Shadow = mp_shadowmap->init(shadow_size, shader_manager);
	return Shadow;
}

//------------------------------------------------------------------------------

void Light::createShadow(const ObjectVec & scene, const Camera &mainCamera)
{
	if (!Shadow)
		return;

	//TODO: complete the function call by sending other arguments.
	mp_shadowmap->create(scene, mainCamera.getWorldView(), Position, gml::add(Direction, Position));
}

//------------------------------------------------------------------------------

void Light::bindShadow(GLenum textureUnit)
{
	if (!Shadow)
		return;

	mp_shadowmap->bindGL(textureUnit);
}

//------------------------------------------------------------------------------

void Light::unbindShadow(GLenum textureUnit)
{
	if (!Shadow)
		return;

	mp_shadowmap->unbindGL(textureUnit);
}

//------------------------------------------------------------------------------

void Light::setType(LightType lt)
{ 
	m_type = lt; mp_shadowmap->setType(lt);
}

//------------------------------------------------------------------------------

gml::mat4x4_t Light::getCamProjectionMatrix ()
{ 
	return mp_shadowmap->getCamProjectionMatrix(); 
}

//==============================================================================

