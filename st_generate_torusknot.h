// by Jari Komppa 2018
// Under unlicense. Google it.
// Use ST_GENERATE_TORUSKNOT_H_IMPLEMENTATION in one source file before including this.
// (Or if you're using a bunch of ST_ headers and want to implement all, just use ST_H_IMPLEMENTATION)
// Needs opengl headers before it, naturally.

// This is a mesh generator, and depends on st_vertexbuffer.h

#ifndef ST_GENERATE_TORUSKNOT_H
#define ST_GENERATE_TORUSKNOT_H

#if !defined(ST_VERTEXBUFFER_H) 
#if defined(ST_H_IMPLEMENTATION)
#error "st_vertexbuffer.h needs to be included before generators"
#endif
#include "st_vertexbuffer.h"
#endif

namespace st
{
	Vertexbuffer *generate_torusknot(
		int aSteps,           // in: Number of steps in the torus knot
		int aFacets,          // in: Number of facets
		float aScale,         // in: Scale of the knot
		float aThickness,     // in: Thickness of the knot
		float aClumps,        // in: Number of clumps in the knot
		float aClumpOffset,   // in: Offset of the clump (in 0..2pi)
		float aClumpScale,    // in: Scale of a clump
		float aUScale,        // in: U coordinate scale
		float aVScale,        // in: V coordinate scale
		float aP,             // in: P parameter of the knot
		float aQ);            // in: Q parameter of the knot
}

#if defined(ST_GENERATE_TORUSKNOT_H_IMPLEMENTATION) || defined(ST_H_IMPLEMENTATION)

namespace st
{
	Vertexbuffer *generate_torusknot(
		int aSteps,           // in: Number of steps in the torus knot
		int aFacets,          // in: Number of facets
		float aScale,         // in: Scale of the knot
		float aThickness,     // in: Thickness of the knot
		float aClumps,        // in: Number of clumps in the knot
		float aClumpOffset,   // in: Offset of the clump (in 0..2pi)
		float aClumpScale,    // in: Scale of a clump
		float aUScale,        // in: U coordinate scale
		float aVScale,        // in: V coordinate scale
		float aP,             // in: P parameter of the knot
		float aQ)             // in: Q parameter of the knot
	{
		int vertices = (aSteps + 1) * (aFacets + 1) + 1;
		int indices = (aSteps + 1) * aFacets * 2;
		
		Vertexbuffer * v = new Vertexbuffer();
		v->init(vertices, indices, 0, GL_TRIANGLE_STRIP);
		int i, j;

		aThickness *= aScale;

		GLfloat *vtx = v->addAttribArray(3, 1);
		GLfloat *normal = v->addAttribArray(3, 2);
		GLfloat *texcoord = v->addAttribArray(2, 3);
		GLuint  *idx = v->mIndices;

		for (j = 0; j < aFacets; j++)
		{
			for (i = 0; i < aSteps + 1; i++)
			{
				idx[i * 2 + 0 + j * (aSteps + 1) * 2] = ((j + 1) + i * (aFacets + 1));
				idx[i * 2 + 1 + j * (aSteps + 1) * 2] = (j + i * (aFacets + 1));
			}
		}

		for (i = 0; i < aSteps; i++)
		{
			float centerpoint[3];
			float Pp = (float)(aP * i * M_PI * 2 / aSteps);
			float Qp = (float)(aQ * i * M_PI * 2 / aSteps);
			float r = (.5f * (2 + (float)sin(Qp))) * aScale;
			centerpoint[0] = r * (float)cos(Pp);
			centerpoint[1] = r * (float)cos(Qp);
			centerpoint[2] = r * (float)sin(Pp);

			float nextpoint[3];
			Pp = (float)(aP * (i + 1) * M_PI*2 / aSteps);
			Qp = (float)(aQ * (i + 1) * M_PI*2 / aSteps);
			r = (.5f * (2 + (float)sin(Qp))) * aScale;
			nextpoint[0] = r * (float)cos(Pp);
			nextpoint[1] = r * (float)cos(Qp);
			nextpoint[2] = r * (float)sin(Pp);

			float T[3];
			T[0] = nextpoint[0] - centerpoint[0];
			T[1] = nextpoint[1] - centerpoint[1];
			T[2] = nextpoint[2] - centerpoint[2];

			float N[3];
			N[0] = nextpoint[0] + centerpoint[0];
			N[1] = nextpoint[1] + centerpoint[1];
			N[2] = nextpoint[2] + centerpoint[2];


			float B[3];
			B[0] = T[1] * N[2] - T[2] * N[1];
			B[1] = T[2] * N[0] - T[0] * N[2];
			B[2] = T[0] * N[1] - T[1] * N[0];

			N[0] = B[1] * T[2] - B[2] * T[1];
			N[1] = B[2] * T[0] - B[0] * T[2];
			N[2] = B[0] * T[1] - B[1] * T[0];

			float l;
			l = (float)sqrt(B[0] * B[0] + B[1] * B[1] + B[2] * B[2]);
			B[0] /= l;
			B[1] /= l;
			B[2] /= l;

			l = (float)sqrt(N[0] * N[0] + N[1] * N[1] + N[2] * N[2]);
			N[0] /= l;
			N[1] /= l;
			N[2] /= l;

			for (j = 0; j < aFacets; j++)
			{
				float pointx = (float)sin(j * M_PI * 2 / aFacets) * aThickness * (((float)sin(aClumpOffset + aClumps * i * M_PI * 2 / aSteps) * aClumpScale) + 1);
				float pointy = (float)cos(j * M_PI * 2 / aFacets) * aThickness * (((float)cos(aClumpOffset + aClumps * i * M_PI * 2 / aSteps) * aClumpScale) + 1);

				vtx[i * (aFacets + 1) * 3 + j * 3 + 0] = N[0] * pointx + B[0] * pointy + centerpoint[0];
				vtx[i * (aFacets + 1) * 3 + j * 3 + 1] = N[1] * pointx + B[1] * pointy + centerpoint[1];
				vtx[i * (aFacets + 1) * 3 + j * 3 + 2] = N[2] * pointx + B[2] * pointy + centerpoint[2];

				normal[i * (aFacets + 1) * 3 + j * 3 + 0] = vtx[i * (aFacets + 1) * 3 + j * 3 + 0] - centerpoint[0];
				normal[i * (aFacets + 1) * 3 + j * 3 + 1] = vtx[i * (aFacets + 1) * 3 + j * 3 + 1] - centerpoint[1];
				normal[i * (aFacets + 1) * 3 + j * 3 + 2] = vtx[i * (aFacets + 1) * 3 + j * 3 + 2] - centerpoint[2];

				float l;
				l = (float)sqrt(normal[i * (aFacets + 1) * 3 + j * 3 + 0] * normal[i * (aFacets + 1) * 3 + j * 3 + 0] +
					normal[i * (aFacets + 1) * 3 + j * 3 + 1] * normal[i * (aFacets + 1) * 3 + j * 3 + 1] +
					normal[i * (aFacets + 1) * 3 + j * 3 + 2] * normal[i * (aFacets + 1) * 3 + j * 3 + 2]);

				normal[i * (aFacets + 1) * 3 + j * 3 + 0] /= l;
				normal[i * (aFacets + 1) * 3 + j * 3 + 1] /= l;
				normal[i * (aFacets + 1) * 3 + j * 3 + 2] /= l;

				texcoord[i * (aFacets + 1) * 2 + j * 2 + 0] = ((float)j / aFacets) * aUScale;
				texcoord[i * (aFacets + 1) * 2 + j * 2 + 1] = ((float)i / aSteps) * aVScale;
			}
			// create duplicate vertex for sideways wrapping
			// otherwise identical to first vertex in the 'ring' except for the U coordinate
			vtx[i * (aFacets + 1) * 3 + aFacets * 3 + 0] = vtx[i * (aFacets + 1) * 3 + 0];
			vtx[i * (aFacets + 1) * 3 + aFacets * 3 + 1] = vtx[i * (aFacets + 1) * 3 + 1];
			vtx[i * (aFacets + 1) * 3 + aFacets * 3 + 2] = vtx[i * (aFacets + 1) * 3 + 2];

			normal[i * (aFacets + 1) * 3 + aFacets * 3 + 0] = normal[i * (aFacets + 1) * 3 + 0];
			normal[i * (aFacets + 1) * 3 + aFacets * 3 + 1] = normal[i * (aFacets + 1) * 3 + 1];
			normal[i * (aFacets + 1) * 3 + aFacets * 3 + 2] = normal[i * (aFacets + 1) * 3 + 2];

			texcoord[i * (aFacets + 1) * 2 + aFacets * 2 + 0] = aUScale;
			texcoord[i * (aFacets + 1) * 2 + aFacets * 2 + 1] = texcoord[i * (aFacets + 1) * 2 + 1];

		}

		// create duplicate ring of vertices for longways wrapping
		// otherwise identical to first 'ring' in the knot except for the V coordinate
		for (j = 0; j < aFacets; j++)
		{
			vtx[aSteps * (aFacets + 1) * 3 + j * 3 + 0] = vtx[j * 3 + 0];
			vtx[aSteps * (aFacets + 1) * 3 + j * 3 + 1] = vtx[j * 3 + 1];
			vtx[aSteps * (aFacets + 1) * 3 + j * 3 + 2] = vtx[j * 3 + 2];

			normal[aSteps * (aFacets + 1) * 3 + j * 3 + 0] = normal[j * 3 + 0];
			normal[aSteps * (aFacets + 1) * 3 + j * 3 + 1] = normal[j * 3 + 1];
			normal[aSteps * (aFacets + 1) * 3 + j * 3 + 2] = normal[j * 3 + 2];

			texcoord[aSteps * (aFacets + 1) * 2 + j * 2 + 0] = texcoord[j * 2 + 0];
			texcoord[aSteps * (aFacets + 1) * 2 + j * 2 + 1] = aVScale;
		}

		// finally, there's one vertex that needs to be duplicated due to both U and V coordinate.

		vtx[aSteps * (aFacets + 1) * 3 + aFacets * 3 + 0] = vtx[0];
		vtx[aSteps * (aFacets + 1) * 3 + aFacets * 3 + 1] = vtx[1];
		vtx[aSteps * (aFacets + 1) * 3 + aFacets * 3 + 2] = vtx[2];

		normal[aSteps * (aFacets + 1) * 3 + aFacets * 3 + 0] = normal[0];
		normal[aSteps * (aFacets + 1) * 3 + aFacets * 3 + 1] = normal[1];
		normal[aSteps * (aFacets + 1) * 3 + aFacets * 3 + 2] = normal[2];

		texcoord[aSteps * (aFacets + 1) * 2 + aFacets * 2 + 0] = aUScale;
		texcoord[aSteps * (aFacets + 1) * 2 + aFacets * 2 + 1] = aVScale;

		return v;
	}
}

#endif // ST_GEMERATE_CUBE_H_IMPLEMENTATION
#endif // ST_GENERATE_CUBE_H