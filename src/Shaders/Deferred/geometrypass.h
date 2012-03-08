/*
 * Shader that only outputs to GBuffer
 */


#pragma once
#ifndef __SHADERS_DEFERRED_GEO_PASS_H_
#define __SHADERS_DEFERRED_GEO_PASS_H_

#include "../shader.h"

namespace Shader
{
namespace Deferred
{

class GeometryPass : public Shader
{
protected:

public:
	GeometryPass();
	virtual ~GeometryPass();

	virtual bool setUniforms(const GLProgUniforms &uniforms, const bool usingShadow=false) const;
};

}
}

#endif
