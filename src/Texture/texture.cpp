
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

#include <cstring>
#include <cstdlib>

#include "texture.h"
#include "../GL3/gl3w.h"
//#include "Decoders/png.h"
#include "../glUtils.h"
#include "../lodepng.h"

namespace Texture
{

GLuint createTexture(const char* filename)
{
	std::vector< unsigned char > rawImage;
	LodePNG::loadFile( rawImage, filename );

	LodePNG::Decoder decoder;
	std::vector< unsigned char > image;
	decoder.decode( image, rawImage.empty() ? 0 : &rawImage[0],
		            (unsigned)rawImage.size() );

	//
	// Flip and invert the PNG image since OpenGL likes to load everything
	// backwards from what is considered normal!
	//

	unsigned char *imagePtr = &image[0];
	int halfTheHeightInPixels = decoder.getHeight() / 2;
	int heightInPixels = decoder.getHeight();

	// Assuming RGBA for 4 components per pixel.
	int numColorComponents = 4;

	// Assuming each color component is an unsigned char.
	int widthInChars = decoder.getWidth() * numColorComponents;

	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;

	for( int h = 0; h < halfTheHeightInPixels; ++h )
	{
		top = imagePtr + h * widthInChars;
		bottom = imagePtr + (heightInPixels - h - 1) * widthInChars;

		for( int w = 0; w < widthInChars; ++w )
		{
		    // Swap the chars around.
		    temp = *top;
		    *top = *bottom;
		    *bottom = temp;

		    ++top;
		    ++bottom;
		}
	}

	GLuint texid;
	//glEnable(GL_TEXTURE_2D);
	glGenTextures( 1, &texid );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture( GL_TEXTURE_2D, texid );

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, decoder.getWidth(),
		          decoder.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
		          &image[0] );

	if (isGLError())
		return -1;

	return texid;
}

Texture::Texture(const char *filename,
		FilterType minFilter, FilterType magFilter,
		WrapMode wrap)
{
/*
	m_filename = strdup(filename);
	m_minFilter = minFilter;
	m_magFilter = magFilter;

	m_wrapMode = wrap;

	m_image = 0;
	m_handle = 0;

	m_isReady = false;

	// Try to read in the image file
	FILE *infile = fopen(filename, "r");
	if (!infile)
	{
		return;
	}

	Image::PNGDecoder decoder;
	if ( !decoder.checkSig(infile) )
	{
		fprintf(stderr, "ERROR! Texture file not a png\n");
		fclose(infile);
		return;
	}

	void *m_image = decoder.decode(infile, m_width, m_height, m_nChannels, m_bitDepth, m_rowBytes);

	if ( !m_image )
	{
		return;
	}

	// Upload the texture to the GL context
	glGenTextures(1, &m_handle);
	if ( isGLError() || (0==m_handle) )
	{
		return;
	}
	glBindTexture(GL_TEXTURE_2D, m_handle);

	if ( isGLError() )
	{
		fprintf(stderr, "ERROR!! 1\n");
		return;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapMode);

	if ( isGLError() )
	{
		fprintf(stderr, "ERROR!! 1\n");
		return;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter);

	if ( isGLError() )
	{
		fprintf(stderr, "ERROR!! 1\n");
		return;
	}

	// Each row of image data is 4-byte aligned.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	if ( isGLError() )
	{
		fprintf(stderr, "ERROR!! 1\n");
		return;
	}

	//fprintf(stderr, "Channels = %u\nwidth = %u\nheight = %u\nbitdepth = %u\n", m_nChannels, m_width, m_height, m_bitDepth);

	glTexImage2D(GL_TEXTURE_2D, 0,
			(m_nChannels==1)?GL_RED:GL_RGB8,
			m_width, m_height, 0,
			(m_nChannels==1)?GL_RED:GL_RGB,
			(m_bitDepth==8)?GL_UNSIGNED_BYTE:GL_UNSIGNED_SHORT,
			m_image);

	if ( isGLError() )
	{
		fprintf(stderr, "ERROR!! 1\n");
		return;
	}

	m_isReady = glIsTexture(m_handle) == GL_TRUE;
*/

	m_handle = createTexture(filename);
	m_isReady = glIsTexture(m_handle) == GL_TRUE;

}

Texture::~Texture()
{
	if (m_filename) free((char*)m_filename);
	if (m_image) free(m_image);
	if (m_handle)
	{
		glDeleteTextures(1, &m_handle);
	}
}


void Texture::bindGL(GLenum textureUnit) const
{
	if (m_isReady)
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, m_handle);
	}
}

}
