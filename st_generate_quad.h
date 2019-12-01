// by Jari Komppa 2019
// Under unlicense. Google it.
// Use ST_GENERATE_QUAD_H_IMPLEMENTATION in one source file before including this.
// (Or if you're using a bunch of ST_ headers and want to implement all, just use ST_H_IMPLEMENTATION)
// Needs opengl headers before it, naturally.

// This is a mesh generator, and depends on st_vertexbuffer.h

#ifndef ST_GENERATE_QUAD_H
#define ST_GENERATE_QUAD_H

#if !defined(ST_VERTEXBUFFER_H) 
#if defined(ST_H_IMPLEMENTATION)
#error "st_vertexbuffer.h needs to be included before generators"
#endif
#include "st_vertexbuffer.h"
#endif

namespace st
{
	Vertexbuffer *generate_quad();
}

#if defined(ST_GENERATE_QUAD_H_IMPLEMENTATION) || defined(ST_H_IMPLEMENTATION)

namespace st
{
	Vertexbuffer* generate_quad()
    {
    	Vertexbuffer* v = new Vertexbuffer();
    	v->init(4, 6);
    	v->mIndices[0] = 0;
    	v->mIndices[1] = 1;
    	v->mIndices[2] = 2;
    	v->mIndices[3] = 1;
    	v->mIndices[4] = 2;
    	v->mIndices[5] = 3;
    	float* vertex = v->addAttribArray(3, 1);
    	vertex[0 * 3 + 0] = -1;
    	vertex[0 * 3 + 1] = -1;
    	vertex[0 * 3 + 2] = 0;

    	vertex[1 * 3 + 0] = 1;
    	vertex[1 * 3 + 1] = -1;
    	vertex[1 * 3 + 2] = 0;

    	vertex[2 * 3 + 0] = -1;
    	vertex[2 * 3 + 1] = 1;
    	vertex[2 * 3 + 2] = 0;

    	vertex[3 * 3 + 0] = 1;
    	vertex[3 * 3 + 1] = 1; 
    	vertex[3 * 3 + 2] = 0;
    	
    	float* uv = v->addAttribArray(2, 3);
    	uv[0 * 2 + 0] = 0;
    	uv[0 * 2 + 1] = 0;

    	uv[1 * 2 + 0] = 1;
    	uv[1 * 2 + 1] = 0;

    	uv[2 * 2 + 0] = 0;
    	uv[2 * 2 + 1] = 1;

    	uv[3 * 2 + 0] = 1;
    	uv[3 * 2 + 1] = 1;
    	
    	return v;
    }
}

#endif // ST_GEMERATE_QUAD_H_IMPLEMENTATION
#endif // ST_GENERATE_QUAD_H