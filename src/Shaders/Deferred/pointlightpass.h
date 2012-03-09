/*
 * Shader that only outputs to GBuffer
 */


#pragma once
#ifndef __SHADERS_DEFERRED_POINGLIGHT_PASS_H_
#define __SHADERS_DEFERRED_POINGLIGHT_PASS_H_

#include "../shader.h"

namespace Shader
{
namespace Deferred
{

class PointLightPass : public Shader
{
protected:

public:
	PointLightPass();
	virtual ~PointLightPass();

	virtual bool setUniforms(const GLProgUniforms &uniforms, const bool usingShadow=false) const;
};

}
}

#endif
