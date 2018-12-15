// by Jari Komppa 2018
// Under unlicense. Google it.
// Use ST_GENERATE_CUBE_H_IMPLEMENTATION in one source file before including this.
// (Or if you're using a bunch of ST_ headers and want to implement all, just use ST_H_IMPLEMENTATION)
// Needs opengl headers before it, naturally.

// This is a mesh generator, and depends on st_vertexbuffer.h

#ifndef ST_GENERATE_CUBE_H
#define ST_GENERATE_CUBE_H

#if !defined(ST_VERTEXBUFFER_H) 
#if defined(ST_H_IMPLEMENTATION)
#error "st_vertexbuffer.h needs to be included before generators"
#endif
#include "st_vertexbuffer.h"
#endif

namespace st
{
	Vertexbuffer *generate_cube(float w, float h, float l);
}

#if defined(ST_GENERATE_CUBE_H_IMPLEMENTATION) || defined(ST_H_IMPLEMENTATION)

namespace st
{
	Vertexbuffer *generate_cube(float w, float h, float l)
	{
		Vertexbuffer * v = new Vertexbuffer();
		v->init(24, 12 * 3);

		int i;
		static GLfloat vtx[] =
		{
		 -1.0f,  1.0f,  1.0f,
		  1.0f,  1.0f,  1.0f,
		 -1.0f, -1.0f,  1.0f,
		  1.0f, -1.0f,  1.0f,

		  1.0f, -1.0f,  1.0f,
		 -1.0f, -1.0f,  1.0f,
		  1.0f, -1.0f, -1.0f,
		 -1.0f, -1.0f, -1.0f,

		 -1.0f, -1.0f, -1.0f,
		  1.0f, -1.0f, -1.0f,
		 -1.0f,  1.0f, -1.0f,
		  1.0f,  1.0f, -1.0f,

		  1.0f,  1.0f, -1.0f,
		 -1.0f,  1.0f, -1.0f,
		  1.0f,  1.0f,  1.0f,
		 -1.0f,  1.0f,  1.0f,

		 -1.0f,  1.0f,  1.0f,
		 -1.0f, -1.0f,  1.0f,
		 -1.0f,  1.0f, -1.0f,
		 -1.0f, -1.0f, -1.0f,

		  1.0f, -1.0f, -1.0f,
		  1.0f, -1.0f,  1.0f,
		  1.0f,  1.0f, -1.0f,
		  1.0f,  1.0f,  1.0f,
		};

		static GLfloat tex[] =
		{
		0,  0,    1,  0,    0,  1,    1,  1,
		0,  0,    1,  0,    0,  1,    1,  1,
		0,  0,    1,  0,    0,  1,    1,  1,
		0,  0,    1,  0,    0,  1,    1,  1,
		0,  0,    1,  0,    0,  1,    1,  1,
		0,  0,    1,  0,    0,  1,    1,  1,

		};

		static GLuint idx[] =
		{
		0,    2,    1,
		1,    2,    3,
		5,    7,    4,
		4,    7,    6,
		8,   10,    9,
		9,   10,   11,
	   12,   13,   14,
	   13,   15,   14,
	   16,   18,   17,
	   17,   18,   19,
	   20,   22,   21,
	   21,   22,   23 };

		for (i = 0; i < 12 * 3; i++)
			v->mIndices[i] = idx[i];

		float *vertex = v->addAttribArray(3, 1);
		for (i = 0; i < 24; i++)
		{
			vertex[i * 3 + 0] = vtx[i * 3 + 0] * w;
			vertex[i * 3 + 1] = vtx[i * 3 + 1] * h;
			vertex[i * 3 + 2] = vtx[i * 3 + 2] * l;
		}

		float *normal = v->addAttribArray(3, 2);
		for (i = 0; i < 24; i++)
		{
			float l = sqrt(vtx[i * 3 + 0] * vtx[i * 3 + 0] +
  						   vtx[i * 3 + 1] * vtx[i * 3 + 1] +
						   vtx[i * 3 + 2] * vtx[i * 3 + 2]);

			normal[i * 3 + 0] = vtx[i * 3 + 0] / l;
			normal[i * 3 + 1] = vtx[i * 3 + 1] / l;
			normal[i * 3 + 2] = vtx[i * 3 + 2] / l;
		}

		float *texcoord = v->addAttribArray(2, 3);
		for (i = 0; i < 24 * 2; i++)
			texcoord[i] = tex[i];

		return v;
	}
}

#endif // ST_GEMERATE_CUBE_H_IMPLEMENTATION
#endif // ST_GENERATE_CUBE_H