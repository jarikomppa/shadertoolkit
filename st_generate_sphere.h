// by Jari Komppa 2018
// Under unlicense. Google it.
// Use ST_GENERATE_SPHERE_H_IMPLEMENTATION in one source file before including this.
// (Or if you're using a bunch of ST_ headers and want to implement all, just use ST_H_IMPLEMENTATION)
// Needs opengl headers before it, naturally.

// This is a mesh generator, and depends on st_vertexbuffer.h

#ifndef ST_GENERATE_SPHERE_H
#define ST_GENERATE_SPHERE_H

#if !defined(ST_VERTEXBUFFER_H) 
#if defined(ST_H_IMPLEMENTATION)
#error "st_vertexbuffer.h needs to be included before generators"
#endif
#include "st_vertexbuffer.h"
#endif

namespace st
{
	Vertexbuffer *generate_sphere(float aScale, int aIters);
}

#if defined(ST_GENERATE_SPHERE_H_IMPLEMENTATION) || defined(ST_H_IMPLEMENTATION)

namespace st
{
	namespace spheregen_impl
	{
		struct VBdata
		{
			GLint mIndices;
			GLint mVertices;
			GLuint *mIndex;
			GLfloat *mVertex;
			GLfloat *mNormal;
			GLfloat *mTexcoord;
			VBdata() : mIndex(0), mVertex(0), mNormal(0), mTexcoord(0) {}
			~VBdata() { delete[] mIndex; delete[] mVertex; delete[] mNormal; delete[] mTexcoord; }
		};

		void tesselate_edge(VBdata * aVB)
		{
			// edge-center tesselate
			//
			//      .
			//     / \
			//    /___\
			//   / \ / \
			//  /___V___\
		
			// new vertex per two indices
			GLfloat * vtx = new GLfloat[(aVB->mVertices + aVB->mIndices / 2) * 3];
			GLfloat * norm = new GLfloat[(aVB->mVertices + aVB->mIndices / 2) * 3];
			// four times the indices
			GLuint  * idx = new GLuint[aVB->mIndices * 4];
			// lookup for edges: an edge per two indices, 32 ints per edge
			int * lookup = new int[(aVB->mIndices / 2) * 32];
			// clear lookup to zeros to begin with
			memset(lookup, 0, sizeof(int) * (aVB->mIndices / 2) * 32);

			int i;
			// copy originals
			for (i = 0; i < aVB->mVertices * 3; i++)
			{
				vtx[i] = aVB->mVertex[i];
				norm[i] = aVB->mNormal[i];
			}

			// create new ones (one per edge, discarding duplicates)
			int newpos = 0;
			for (i = 0; i < aVB->mIndices / 3; i++)
			{
				int j;
				for (j = 0; j < 3; j++)
				{
					int pa = aVB->mIndex[i * 3 + ((j + 0) % 3)];
					int pb = aVB->mIndex[i * 3 + ((j + 1) % 3)];

					if (pa > pb)
					{
						int temp = pa;
						pa = pb;
						pb = temp;
					}

					int found = 0;
					int k;

					for (k = 0; !found && k < 16; k++)
						found = (lookup[pa * 32 + k] == pb);

					if (!found)
					{
						float nx = (aVB->mVertex[pa * 3 + 0] + aVB->mVertex[pb * 3 + 0]) / 2;
						float ny = (aVB->mVertex[pa * 3 + 1] + aVB->mVertex[pb * 3 + 1]) / 2;
						float nz = (aVB->mVertex[pa * 3 + 2] + aVB->mVertex[pb * 3 + 2]) / 2;
						norm[(newpos + aVB->mVertices) * 3 + 0] = vtx[(newpos + aVB->mVertices) * 3 + 0] = nx;
						norm[(newpos + aVB->mVertices) * 3 + 1] = vtx[(newpos + aVB->mVertices) * 3 + 1] = ny;
						norm[(newpos + aVB->mVertices) * 3 + 2] = vtx[(newpos + aVB->mVertices) * 3 + 2] = nz;
						k = 0;
						while (lookup[pa * 32 + k] != 0 && k < 16) k++;
						lookup[pa * 32 + k] = pb;
						lookup[pa * 32 + k + 16] = newpos;
						newpos++;
					}
				}
			}

			// create new indices
			int idxpos = 0;
			for (i = 0; i < aVB->mIndices / 3; i++)
			{
				// first, find the mid-edge verts for all triangles
				int edgevtxs[3];
				int j;
				for (j = 0; j < 3; j++)
				{
					int pa = aVB->mIndex[i * 3 + ((j + 0) % 3)];
					int pb = aVB->mIndex[i * 3 + ((j + 1) % 3)];

					if (pa > pb)
					{
						int temp = pa;
						pa = pb;
						pb = temp;
					}

					int found = 0;
					int edgevtx = -1;
					int k = 0;
					while (lookup[pa * 32 + k] != pb && k < 16) k++;

					edgevtx = lookup[pa * 32 + k + 16] + aVB->mVertices;
					edgevtxs[j] = edgevtx;
				}
				// next, create triangles, careful with the vertex 
				// order to preserve backface culling
				idx[idxpos++] = aVB->mIndex[i * 3 + 0];
				idx[idxpos++] = edgevtxs[0];
				idx[idxpos++] = edgevtxs[2];

				idx[idxpos++] = edgevtxs[1];
				idx[idxpos++] = aVB->mIndex[i * 3 + 2];
				idx[idxpos++] = edgevtxs[2];

				idx[idxpos++] = edgevtxs[1];
				idx[idxpos++] = edgevtxs[0];
				idx[idxpos++] = aVB->mIndex[i * 3 + 1];

				// the new center triangle
				idx[idxpos++] = edgevtxs[0];
				idx[idxpos++] = edgevtxs[1];
				idx[idxpos++] = edgevtxs[2];
			}

			// free the old junk
			delete[] lookup;
			delete[] aVB->mIndex;
			delete[] aVB->mNormal;
			delete[] aVB->mVertex;

			// replace pointers
			aVB->mIndex = idx;
			aVB->mNormal = norm;
			aVB->mVertex = vtx;

			// set the counts
			aVB->mVertices += newpos;
			aVB->mIndices = idxpos;

			// done!
		}

		void normalize(VBdata *aVB, float aScale)
		{
			int i;
			for (i = 0; i < aVB->mVertices; i++)
			{
				float l = sqrt(aVB->mVertex[i * 3 + 0] * aVB->mVertex[i * 3 + 0] +
					aVB->mVertex[i * 3 + 1] * aVB->mVertex[i * 3 + 1] +
					aVB->mVertex[i * 3 + 2] * aVB->mVertex[i * 3 + 2]);

				aVB->mNormal[i * 3 + 0] = aVB->mVertex[i * 3 + 0] / l;
				aVB->mNormal[i * 3 + 1] = aVB->mVertex[i * 3 + 1] / l;
				aVB->mNormal[i * 3 + 2] = aVB->mVertex[i * 3 + 2] / l;
			}

			for (i = 0; i < aVB->mVertices; i++)
			{
				aVB->mVertex[i * 3 + 0] = aVB->mNormal[i * 3 + 0] * aScale;
				aVB->mVertex[i * 3 + 1] = aVB->mNormal[i * 3 + 1] * aScale;
				aVB->mVertex[i * 3 + 2] = aVB->mNormal[i * 3 + 2] * aScale;
			}

		}

		void fix_uv(VBdata *aVB)
		{
			// We have a slight problem: the UV coordinates wrap around when they 
			// get around the sphere. Soo.. we need to find the problematic vertices 
			// and duplicate them.

			// we don't have any idea just how much space we'll need, so let's
			// make the buffers huge enough to make sure.
			GLfloat * vtx = new GLfloat[aVB->mVertices * 8 * 3];
			GLfloat * norm = new GLfloat[aVB->mVertices * 8 * 3];
			GLfloat * uv = new GLfloat[aVB->mVertices * 8 * 2];

			memcpy(vtx, aVB->mVertex, sizeof(GLfloat) * 3 * aVB->mVertices);
			memcpy(norm, aVB->mNormal, sizeof(GLfloat) * 3 * aVB->mVertices);
			memcpy(uv, aVB->mTexcoord, sizeof(GLfloat) * 2 * aVB->mVertices);

			// find cap vertices
			int i;
			float cap0v = 1.0f;
			float cap1v = 0.0f;
			for (i = 0; i < aVB->mVertices; i++)
			{
				if (uv[i * 2 + 1] < cap0v)
				{
					cap0v = uv[i * 2 + 1];
				}
				if (uv[i * 2 + 1] > cap1v)
				{
					cap1v = uv[i * 2 + 1];
				}
			}

			int newidx = aVB->mVertices;

			for (i = 0; i < aVB->mIndices / 3; i++)
			{
				int j;
				for (j = 0; j < 3; j++)
				{
					if ((uv[aVB->mIndex[i * 3 + j] * 2 + 0] - uv[aVB->mIndex[i * 3 + ((j + 1) % 3)] * 2 + 0]) > 0.25f ||
						(uv[aVB->mIndex[i * 3 + j] * 2 + 0] - uv[aVB->mIndex[i * 3 + ((j + 2) % 3)] * 2 + 0]) > 0.25f)
					{
						// duplicate this vertex
						vtx[newidx * 3 + 0] = vtx[aVB->mIndex[i * 3 + j] * 3 + 0];
						vtx[newidx * 3 + 1] = vtx[aVB->mIndex[i * 3 + j] * 3 + 1];
						vtx[newidx * 3 + 2] = vtx[aVB->mIndex[i * 3 + j] * 3 + 2];
						norm[newidx * 3 + 0] = norm[aVB->mIndex[i * 3 + j] * 3 + 0];
						norm[newidx * 3 + 1] = norm[aVB->mIndex[i * 3 + j] * 3 + 1];
						norm[newidx * 3 + 2] = norm[aVB->mIndex[i * 3 + j] * 3 + 2];
						uv[newidx * 2 + 0] = uv[aVB->mIndex[i * 3 + j] * 2 + 0];
						uv[newidx * 2 + 1] = uv[aVB->mIndex[i * 3 + j] * 2 + 1];

						uv[newidx * 2 + 0] -= 1.0f;

						aVB->mIndex[i * 3 + j] = newidx;
						newidx++;
					}

					if (uv[aVB->mIndex[i * 3 + j] * 2 + 1] == cap0v ||
						uv[aVB->mIndex[i * 3 + j] * 2 + 1] == cap1v)
					{
						// cap vertex - need to duplicate and recalculate UV
						// (this will leak two verts, but who cares)
						// duplicate this vertex

						vtx[newidx * 3 + 0] = vtx[aVB->mIndex[i * 3 + j] * 3 + 0];
						vtx[newidx * 3 + 1] = vtx[aVB->mIndex[i * 3 + j] * 3 + 1];
						vtx[newidx * 3 + 2] = vtx[aVB->mIndex[i * 3 + j] * 3 + 2];
						norm[newidx * 3 + 0] = norm[aVB->mIndex[i * 3 + j] * 3 + 0];
						norm[newidx * 3 + 1] = norm[aVB->mIndex[i * 3 + j] * 3 + 1];
						norm[newidx * 3 + 2] = norm[aVB->mIndex[i * 3 + j] * 3 + 2];
						uv[newidx * 2 + 0] = (uv[aVB->mIndex[i * 3 + ((j + 1) % 3)] * 2 + 0] + uv[aVB->mIndex[i * 3 + ((j + 2) % 3)] * 2 + 0]) / 2;
						uv[newidx * 2 + 1] = uv[aVB->mIndex[i * 3 + j] * 2 + 1];

						// a small hack to fix a bug that occurs in some iterations
						if ((uv[newidx * 2 + 0] - uv[aVB->mIndex[i * 3 + ((j + 1) % 3)] * 2 + 0]) > 0.25f ||
							(uv[newidx * 2 + 0] - uv[aVB->mIndex[i * 3 + ((j + 2) % 3)] * 2 + 0]) > 0.25f)
							uv[newidx * 2 + 0] -= 0.5f;

						aVB->mIndex[i * 3 + j] = newidx;
						newidx++;
					}
				}
			}

			delete[] aVB->mTexcoord;
			delete[] aVB->mNormal;
			delete[] aVB->mVertex;

			// replace pointers
			aVB->mTexcoord = new GLfloat[newidx * 2];
			aVB->mNormal = new GLfloat[newidx * 3];
			aVB->mVertex = new GLfloat[newidx * 3];

			memcpy(aVB->mVertex, vtx, sizeof(GLfloat) * 3 * newidx);
			memcpy(aVB->mNormal, norm, sizeof(GLfloat) * 3 * newidx);
			memcpy(aVB->mTexcoord, uv, sizeof(GLfloat) * 2 * newidx);

			delete[] vtx;
			delete[] norm;
			delete[] uv;

			// set the counts
			aVB->mVertices = newidx;
		}
	}

	Vertexbuffer *generate_sphere(float aScale, int aIters)
	{
		int i;

		static GLfloat vtx[] =
		{
			0.0f, -1.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			-1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 1.0f, 0.0f
		};

		static GLuint idx[] =
		{
			0,    1,    2,
			0,    2,    3,
			0,    3,    4,
			0,    4,    1,
			1,    5,    2,
			2,    5,    3,
			3,    5,    4,
			4,    5,    1
		};

		spheregen_impl::VBdata vb;

		vb.mIndices = 8 * 3;
		vb.mVertices = 6;

		vb.mIndex = new GLuint[vb.mIndices];
		for (i = 0; i < vb.mIndices; i++)
			vb.mIndex[i] = idx[i];

		vb.mNormal = new GLfloat[vb.mVertices * 3];
		for (i = 0; i < vb.mVertices; i++)
		{
			float l = sqrt(
				vtx[i * 3 + 0] * vtx[i * 3 + 0] +
				vtx[i * 3 + 1] * vtx[i * 3 + 1] +
				vtx[i * 3 + 2] * vtx[i * 3 + 2]);

			vb.mNormal[i * 3 + 0] = vtx[i * 3 + 0] / l;
			vb.mNormal[i * 3 + 1] = vtx[i * 3 + 1] / l;
			vb.mNormal[i * 3 + 2] = vtx[i * 3 + 2] / l;
		}

		vb.mVertex = new GLfloat[vb.mVertices * 3];

		for (i = 0; i < vb.mVertices; i++)
		{
			vb.mVertex[i * 3 + 0] = vb.mNormal[i * 3 + 0] * aScale;
			vb.mVertex[i * 3 + 1] = vb.mNormal[i * 3 + 1] * aScale;
			vb.mVertex[i * 3 + 2] = vb.mNormal[i * 3 + 2] * aScale;
		}

		for (i = 1; i < aIters; i++)
		{
			spheregen_impl::tesselate_edge(&vb);
		}

		spheregen_impl::normalize(&vb, aScale);

		vb.mTexcoord = new GLfloat[vb.mVertices * 2];

		for (i = 0; i < vb.mVertices; i++)
		{
			float l = sqrt(
				vb.mVertex[i * 3 + 0] * vb.mVertex[i * 3 + 0] +
				vb.mVertex[i * 3 + 1] * vb.mVertex[i * 3 + 1] +
				vb.mVertex[i * 3 + 2] * vb.mVertex[i * 3 + 2]);
			vb.mTexcoord[i * 2 + 1] = (float)(acos(vb.mVertex[i * 3 + 1] / l) / M_PI);
			vb.mTexcoord[i * 2 + 0] = (float)((atan2(vb.mVertex[i * 3 + 2], -vb.mVertex[i * 3 + 0]) + M_PI) / (M_PI * 2));
		}

		spheregen_impl::fix_uv(&vb);

		Vertexbuffer *v = new Vertexbuffer;
		v->init(vb.mVertices, vb.mIndices);
		memcpy(v->mIndices, vb.mIndex, sizeof(GLuint) * vb.mIndices);
		float *vertex = v->addAttribArray(3, 1);
		memcpy(vertex, vb.mVertex, sizeof(float) * 3 * vb.mVertices);
		float *normal = v->addAttribArray(3, 2);
		memcpy(normal, vb.mNormal, sizeof(float) * 3 * vb.mVertices);
		float *texcoord = v->addAttribArray(2, 3);
		memcpy(texcoord, vb.mTexcoord, sizeof(float) * 2 * vb.mVertices);

		return v;
	}
}

#endif // ST_GEMERATE_CUBE_H_IMPLEMENTATION
#endif // ST_GENERATE_CUBE_H