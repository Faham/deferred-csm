
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

#include <gl3/gl3w.h>
#include <cstdio>

#include <shaders/deferred/directionallightpass.h>
#include <glUtils.h>
#include <config.h>

namespace Shader
{
namespace Deferred
{

static const char vertShader[] =
		"#version 330\n"
		"uniform mat4 " UNIF_MODELVIEW ";\n"
		"uniform mat4 " UNIF_PROJECTION ";\n"
		"layout (location=0) in vec3 position;\n"
		"void main(void) {\n"
		" gl_Position = " UNIF_PROJECTION " * " UNIF_MODELVIEW " * vec4(position, 1.0);\n"
		"}";
static const char fragShader[] =
		"#version 330\n"
		"uniform vec3 " UNIF_LIGHTRAD ";\n"
		"uniform float " UNIF_DS_AMBIENTINTENCITY ";\n"
		"uniform float " UNIF_DS_DIFFUSEINTENSITY ";\n"
		"uniform vec3 " UNIF_DS_DLDIRECTION ";\n"
		"uniform vec2 " UNIF_DS_SCREENSIZE ";\n"
		"uniform sampler2D " UNIF_DS_POSTEX ";\n"
		"uniform sampler2D " UNIF_DS_DIFFTEX ";\n"
		"uniform sampler2D " UNIF_DS_NORMTEX ";\n"
#if defined (DO_SHADOW)
		"uniform samplerCubeShadow " UNIF_SHADOWMAP ";\n"
#endif
		"out vec4 FragColor;\n"
		"void main(void) {\n"
			"vec2 TexCoord = gl_FragCoord.xy / " UNIF_DS_SCREENSIZE ";\n"
			"vec3 WorldPos = texture(" UNIF_DS_POSTEX ", TexCoord).xyz;\n"
			"vec3 Color = texture(" UNIF_DS_DIFFTEX ", TexCoord).xyz;\n"
			"vec3 Normal = texture(" UNIF_DS_NORMTEX ", TexCoord).xyz;\n"

			// Lighting Internals
			"vec3 AmbientColor = " UNIF_LIGHTRAD " * " UNIF_DS_AMBIENTINTENCITY ";\n"

#if defined (DO_SHADOW)
			"float notShadow = texture(" UNIF_SHADOWMAP ", vec4(-" UNIF_DS_DLDIRECTION ", 0.0f));\n"
			//"float DiffuseFactor = notShadow * dot(Normal, -" UNIF_DS_DLDIRECTION ");\n"
			"float DiffuseFactor = dot(Normal, -" UNIF_DS_DLDIRECTION ");\n"
#else
			"float DiffuseFactor = dot(Normal, -" UNIF_DS_DLDIRECTION ");\n"
#endif
			"vec3 DiffuseColor  = vec3(0, 0, 0);\n"
			"vec3 SpecularColor = vec3(0, 0, 0);\n"

			"if (DiffuseFactor > 0) {\n"
				"DiffuseColor = " UNIF_LIGHTRAD " * " UNIF_DS_DIFFUSEINTENSITY " * DiffuseFactor;\n"

				//TODO: find and replace gEyeWorldPos, gSpecularPower and gMatSpecularIntensity
				//"vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos);\n"
				"vec3 VertexToEye = normalize(- WorldPos);\n"
				"float gMatSpecularIntensity = 0.70f;\n"
				"float gSpecularPower = 0.90f;\n"

				"vec3 LightReflect = normalize(reflect(" UNIF_DS_DLDIRECTION ", Normal));\n"
				"float SpecularFactor = dot(VertexToEye, LightReflect);\n"
#if defined (DO_SHADOW)
				"SpecularFactor = notShadow * pow(SpecularFactor, gSpecularPower);\n"
#else
				"SpecularFactor = pow(SpecularFactor, gSpecularPower);\n"
#endif
				"if (SpecularFactor > 0) {\n"
					"SpecularColor = " UNIF_LIGHTRAD " * gMatSpecularIntensity * SpecularFactor;\n"
				"}\n"
			"}\n"
			//

			"FragColor = vec4(Color, 1.0) * (AmbientColor + DiffuseColor + SpecularColor);\n"
			
			//"FragColor.x += 1.0;\n"
		"}";

DirectionalLightPass::DirectionalLightPass()
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
			(m_program.getUniformID(UNIFORM_LIGHTRAD) >= 0) &&
			(m_program.getUniformID(UNIFORM_DS_AMBIENTINTENCITY) >= 0) &&
			(m_program.getUniformID(UNIFORM_DS_DIFFUSEINTENSITY) >= 0) &&
			(m_program.getUniformID(UNIFORM_DS_DLDIRECTION) >= 0) &&
			(m_program.getUniformID(UNIFORM_DS_SCREENSIZE) >= 0) &&
			(m_program.getUniformID(UNIFORM_DS_POSTEX) >= 0) &&
			(m_program.getUniformID(UNIFORM_DS_DIFFTEX) >= 0) &&
			(m_program.getUniformID(UNIFORM_DS_NORMTEX) >= 0)
#if defined (DO_SHADOW)
			&& (m_program.getUniformID(UNIFORM_SHADOWMAP) >= 0)
#endif
			;
}

DirectionalLightPass::~DirectionalLightPass() {}


bool DirectionalLightPass::setUniforms(const GLProgUniforms &uniforms, const bool usingShadow) const
{
    glUniformMatrix4fv(m_program.getUniformID(UNIFORM_MODELVIEW), 1, GL_FALSE, (GLfloat*)&uniforms.m_modelView);
    glUniformMatrix4fv(m_program.getUniformID(UNIFORM_PROJECTION), 1, GL_FALSE, (GLfloat*)&uniforms.m_projection);
	glUniform3fv(m_program.getUniformID(UNIFORM_LIGHTRAD), 1, (GLfloat*)&uniforms.m_lightRad);
	glUniform1f(m_program.getUniformID(UNIFORM_DS_AMBIENTINTENCITY), uniforms.m_ds_AmbientIntensity);
	glUniform1f(m_program.getUniformID(UNIFORM_DS_DIFFUSEINTENSITY), uniforms.m_ds_DiffuseIntensity);
	glUniform3fv(m_program.getUniformID(UNIFORM_DS_DLDIRECTION), 1, (GLfloat*)&uniforms.m_ds_DirectionalLightDirection);
    glUniform2fv(m_program.getUniformID(UNIFORM_DS_SCREENSIZE), 1, (GLfloat*)&uniforms.m_ds_ScreenSize);
	glUniform1i(m_program.getUniformID(UNIFORM_DS_POSTEX), 0);
	glUniform1i(m_program.getUniformID(UNIFORM_DS_DIFFTEX), 1);
	glUniform1i(m_program.getUniformID(UNIFORM_DS_NORMTEX), 2);
#if defined (DO_SHADOW)
	glUniform1i(m_program.getUniformID(UNIFORM_SHADOWMAP), 3);
#endif

 	return !isGLError();
}

}
}
