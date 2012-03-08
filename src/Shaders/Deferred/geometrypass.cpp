
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

#include "../../GL3/gl3w.h"
#include <cstdio>

#include "geometrypass.h"
#include "../../glUtils.h"

namespace Shader
{
namespace Deferred
{

static const char vertShader[] =
		"#version 330\n"
		"uniform mat4 " UNIF_MODELVIEW ";\n"
		"uniform mat4 " UNIF_PROJECTION ";\n"
		"uniform mat4 " UNIF_NORMALTRANS ";\n"
		"layout (location=0) in vec3 position;\n"
		"layout (location=1) in vec3 normal;\n"
		"layout (location=2) in vec2 texCoord;\n"
		"smooth out vec3 o_position;\n"
		"smooth out vec2 o_texCoord;\n"
		"smooth out vec3 o_normal;\n"
		"void main(void) {\n"
		" vec4 p = " UNIF_MODELVIEW " * vec4(position, 1.0);\n"
		" gl_Position = " UNIF_PROJECTION " * p;\n"
		// TODO: I'm not sure about the transformation of position. It should be in world coordinate or ModelView?
		" o_position = p.xyz;\n"
		" o_texCoord = texCoord;\n"
		" o_normal = (" UNIF_NORMALTRANS " * vec4(normal,0.0)).xyz;\n"
		"}";
static const char fragShader[] =
		"#version 330\n"
		"uniform sampler2D " UNIF_TEXTURE0 ";\n"
		"in vec3 o_position;\n"
		"in vec2 o_texCoord;\n"
		"in vec3 o_normal;\n"
		"out vec3 Position;\n"
		"out vec3 Diffuse;\n"
		"out vec3 Normal;\n"
		"out vec3 TexCoord;\n"
		"void main(void) {\n"
		" Position = o_position;\n"
		" Diffuse = texture(" UNIF_TEXTURE0 ", o_texCoord).xyz;\n"
		" Normal = normalize(o_normal);\n"
		" TexCoord = vec3(o_texCoord, 0.0).xyz;\n"
		"}";

GeometryPass::GeometryPass()
{
	// Try to create, compile, & link a GLSL program using the source
	// you give it.
	if ( !m_program.init(vertShader, fragShader) || isGLError() )
	{
		fprintf(stderr, "ERROR: Depth failed to initialize\n");
	}
	// Make sure that every uniform that you are using in your shader
	// is given a handle.
	// variable names that do not correspond with a uniform will have
	// been given the value -1
	m_isReady =
			(m_program.getUniformID(UNIFORM_MODELVIEW) >= 0) &&
			(m_program.getUniformID(UNIFORM_PROJECTION) >= 0) &&
			(m_program.getUniformID(UNIFORM_NORMALTRANS) >= 0) &&
			(m_program.getUniformID(UNIFORM_TEXTURE0) >= 0);
}
GeometryPass::~GeometryPass() {}


bool GeometryPass::setUniforms(const GLProgUniforms &uniforms, const bool usingShadow) const
{
	glUniform1i(m_program.getUniformID(UNIFORM_TEXTURE0), 0);
    glUniformMatrix4fv(m_program.getUniformID(UNIFORM_MODELVIEW), 1, GL_FALSE, (GLfloat*)&uniforms.m_modelView);
    glUniformMatrix4fv(m_program.getUniformID(UNIFORM_PROJECTION), 1, GL_FALSE, (GLfloat*)&uniforms.m_projection);
	glUniformMatrix4fv(m_program.getUniformID(UNIFORM_NORMALTRANS), 1, GL_FALSE, (GLfloat*)&uniforms.m_normalTrans);

 	return !isGLError();
}

}
}
