/*
 * Shader that only outputs to GBuffer
 */


#pragma once
#ifndef __SHADERS_DEFERRED_DIRLIGHT_PASS_H_
#define __SHADERS_DEFERRED_DIRLIGHT_PASS_H_

#include "../shader.h"

namespace Shader
{
namespace Deferred
{

class DirectionalLightPass : public Shader
{
protected:

public:
	DirectionalLightPass();
	virtual ~DirectionalLightPass();

	virtual bool setUniforms(const GLProgUniforms &uniforms, const bool usingShadow=false) const;
};

}
}

#endif
