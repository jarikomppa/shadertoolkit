// by Jari Komppa 2018
// Under unlicense. Google it.
// Use ST_TEXTURE_H_IMPLEMENTATION in one source file before including this.
// Needs opengl headers before it, naturally.

#ifndef ST_TEXTURE_H
#define ST_TEXTURE_H

namespace st
{
	GLuint loadtexture(const char *aFilename, bool aWrap = true, bool aMipmaps = true);
}

#ifdef ST_TEXTURE_H_IMPLEMENTATION

#undef STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace st
{
	GLuint loadtexture(const char *aFilename, bool aWrap, bool aMipmaps)
	{
		GLuint texname;
		glGenTextures(1, &texname);
		glBindTexture(GL_TEXTURE_2D, texname);

		int x, y, n;
		unsigned char *data = stbi_load(aFilename, &x, &y, &n, 4);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)data);
		if (aWrap)
		{
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		else
		{
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		}
		if (aMipmaps)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		return texname;
	}
}

#endif // ST_VERTEXBUFFER_H_IMPLEMENTATION

#endif // ST_VERTEXBUFFER_H