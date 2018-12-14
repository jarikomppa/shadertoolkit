// by Jari Komppa 2018
// Under unlicense. Google it.
// Use ST_VERTEXBUFFER_H_IMPLEMENTATION in one source file before including this.
// Needs opengl headers before it, naturally.

#ifndef ST_VERTEXBUFFER_H
#define ST_VERTEXBUFFER_H

namespace st
{
	class Vertexbuffer
	{
	protected:
		class Attribarray
		{
		public:
			Attribarray *mNext;
			int mElements;
			int mDivisor;
			GLuint mVBOHandle;
			int mIndex;
			int mFloatCount;
			float *mRawdata;
			Attribarray();
			virtual ~Attribarray();
		};
	public:
		int mPrimitiveType;
		GLuint mVAOHandle;
		int mVertexCount;
		Attribarray *mAttribarray;
		int mDirty;
		int mInstanceCount;
		int mIndexCount;
		GLuint *mIndices;
		GLuint mIndexVBO;
	public:
		float *addAttribArray(int aElements, int aIndex, int aDivisor = 0);
		float *getAttribArrayPtr(int aIndex);
		int getAttribArrayFloatSize(int aIndex);
		int getAttribArrayElementCount(int aIndex);
		void init(int aVertexCount, int aIndexCount = -1, int aInstanceCount = 0, int aPrimitiveType = GL_TRIANGLES);
		void enable();
		void disable();
		void render();
		void optimize();
		virtual ~Vertexbuffer();
		Vertexbuffer();
		void check_gl_error(const char * fn, int l);
	};
}

#ifdef ST_VERTEXBUFFER_H_IMPLEMENTATION

namespace st
{

#define VERTEXBUFFER_H_CHECK_GL_ERROR check_gl_error(__FILE__,__LINE__);

	void Vertexbuffer::check_gl_error(const char * fn, int l)
	{
		int err = glGetError();
		if (err != GL_NO_ERROR)
		{
			err = err;
			char * errst = (char*)"";
			char * enumst = (char*)"";
			switch (err)
			{
			case GL_NO_ERROR: enumst = (char*)"GL_NO_ERROR"; errst = (char*)
				"No error has been recorded. "
				"The value of this symbolic constant is guaranteed to be 0.";
				break;

			case GL_INVALID_ENUM: enumst = (char*)"GL_INVALID_ENUM"; errst = (char*)
				"An unacceptable value is specified for an enumerated argument. "
				"The offending command is ignored "
				"and has no other side effect than to set the error flag.";
				break;

			case GL_INVALID_VALUE: enumst = (char*) "GL_INVALID_VALUE"; errst = (char*)
				"A numeric argument is out of range. "
				"The offending command is ignored "
				"and has no other side effect than to set the error flag.";
				break;

			case GL_INVALID_OPERATION: enumst = (char*)"GL_INVALID_OPERATION"; errst = (char*)
				"The specified operation is not allowed in the current state. "
				"The offending command is ignored "
				"and has no other side effect than to set the error flag.";
				break;

			case GL_STACK_OVERFLOW: enumst = (char*)"GL_STACK_OVERFLOW"; errst = (char*)
				"This command would cause a stack overflow. "
				"The offending command is ignored "
				"and has no other side effect than to set the error flag.";
				break;

			case GL_STACK_UNDERFLOW: enumst = (char*)"GL_STACK_UNDERFLOW"; errst = (char*)
				"This command would cause a stack underflow. "
				"The offending command is ignored "
				"and has no other side effect than to set the error flag.";
				break;

			case GL_OUT_OF_MEMORY: enumst = (char*)"GL_OUT_OF_MEMORY"; errst = (char*)
				"There is not enough memory left to execute the command. "
				"The state of the GL is undefined, "
				"except for the state of the error flags, "
				"after this error is recorded.";
				break;
				/*
			case GL_TABLE_TOO_LARGE: enumst = "GL_TABLE_TOO_LARGE"; errst =


				"The specified table exceeds the implementation's maximum supported table "
				"size.  The offending command is ignored and has no other side effect "
				"than to set the error flag.";
				break;
				*/
			default:
				enumst = (char*)"Unknown"; errst = (char*)"Unknown gl error";
				break;
			}
			char tmp[256];
			sprintf_s(tmp, 256, "%s::%d:%s", fn, l, enumst);
			MessageBoxA(NULL, errst, tmp, MB_OK);
		}
	}

	Vertexbuffer::Attribarray::Attribarray()
	{
		mRawdata = 0;
		mNext = 0;
	}

	Vertexbuffer::Attribarray::~Attribarray()
	{
		delete[] mRawdata;
	}

	Vertexbuffer::Vertexbuffer()
	{
		mIndices = 0;
		mAttribarray = 0;
		glGenVertexArrays(1, &mVAOHandle); VERTEXBUFFER_H_CHECK_GL_ERROR
	}
	
	Vertexbuffer::~Vertexbuffer()
	{
		Attribarray *walker = mAttribarray;
		while (walker)
		{
			Attribarray *p = walker;
			walker = walker->mNext;
			delete[] p->mRawdata;
			glDeleteBuffers(1, &p->mVBOHandle);
			delete p;
		}
		glDeleteVertexArrays(1, &mVAOHandle); VERTEXBUFFER_H_CHECK_GL_ERROR
		delete[] mIndices;
		glDeleteBuffers(1, &mIndexVBO); VERTEXBUFFER_H_CHECK_GL_ERROR
	}

	float *Vertexbuffer::addAttribArray(int aElements, int aIndex, int aDivisor)
	{
		mDirty = 1;
		Attribarray * a = new Attribarray;
		a->mElements = aElements;
		a->mIndex = aIndex;
		a->mDivisor = aDivisor;
		a->mNext = mAttribarray;
		a->mFloatCount = aElements * mVertexCount;
		if (aDivisor)
		{
			a->mFloatCount = aElements * (mInstanceCount / aDivisor);
		}
		a->mRawdata = new float[a->mFloatCount];
		glGenBuffers(1, &a->mVBOHandle); VERTEXBUFFER_H_CHECK_GL_ERROR
		mAttribarray = a;
		return a->mRawdata;
	}

	float *Vertexbuffer::getAttribArrayPtr(int aIndex)
	{
		Attribarray *walker = mAttribarray;
		mDirty = 1;
		while (walker)
		{
			if (walker->mIndex == aIndex)
				return walker->mRawdata;
			walker = walker->mNext;
		}
		return 0;
	}

	int Vertexbuffer::getAttribArrayFloatSize(int aIndex)
	{
		Attribarray *walker = mAttribarray;
		mDirty = 1;
		while (walker)
		{
			if (walker->mIndex == aIndex)
				return walker->mFloatCount;
			walker = walker->mNext;
		}
		return 0;
	}

	int Vertexbuffer::getAttribArrayElementCount(int aIndex)
	{
		Attribarray *walker = mAttribarray;
		mDirty = 1;
		while (walker)
		{
			if (walker->mIndex == aIndex)
				return walker->mElements;
			walker = walker->mNext;
		}
		return 0;
	}

	void Vertexbuffer::init(int aVertexCount, int aIndexCount, int aInstanceCount, int aPrimitivetype)
	{
		mVertexCount = aVertexCount;
		mPrimitiveType = aPrimitivetype;
		mInstanceCount = aInstanceCount;
		mIndexCount = aIndexCount;
		if (mIndexCount == -1)
			mIndexCount = mVertexCount;
		glGenBuffers(1, &mIndexVBO);
		mIndices = new GLuint[mIndexCount];
		int i;
		for (i = 0; i < mIndexCount; i++)
		{
			mIndices[i] = i;
		}
		mDirty = 1;
	}

	void Vertexbuffer::enable()
	{
		if (mDirty)
		{
			optimize();
		}
		glBindVertexArray(mVAOHandle); VERTEXBUFFER_H_CHECK_GL_ERROR
		Attribarray *walker = mAttribarray;
		while (walker)
		{
			glEnableVertexAttribArray(walker->mIndex); VERTEXBUFFER_H_CHECK_GL_ERROR
			glBindBuffer(GL_ARRAY_BUFFER, walker->mVBOHandle); VERTEXBUFFER_H_CHECK_GL_ERROR
			glVertexAttribPointer(walker->mIndex, walker->mElements, GL_FLOAT, 0, 0, 0); VERTEXBUFFER_H_CHECK_GL_ERROR
			glVertexAttribDivisor(walker->mIndex, walker->mDivisor); VERTEXBUFFER_H_CHECK_GL_ERROR
			walker = walker->mNext;
		}
	}
	
	void Vertexbuffer::disable()
	{
		Attribarray *walker = mAttribarray;
		while (walker)
		{
			glDisableVertexAttribArray(walker->mIndex); VERTEXBUFFER_H_CHECK_GL_ERROR
			walker = walker->mNext;
		}
		glBindVertexArray(0); VERTEXBUFFER_H_CHECK_GL_ERROR
	}

	void Vertexbuffer::render()
	{
		if (mInstanceCount > 1)
		{
			glDrawElementsInstanced(mPrimitiveType, mIndexCount, GL_UNSIGNED_INT, 0, mInstanceCount); VERTEXBUFFER_H_CHECK_GL_ERROR
		}
		else
		{
			glDrawElements(mPrimitiveType, mIndexCount, GL_UNSIGNED_INT, 0); VERTEXBUFFER_H_CHECK_GL_ERROR
		}
	}
	
	void Vertexbuffer::optimize()
	{
		glBindVertexArray(mVAOHandle); VERTEXBUFFER_H_CHECK_GL_ERROR
		mDirty = 0;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexVBO); VERTEXBUFFER_H_CHECK_GL_ERROR
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexCount * sizeof(GLuint), mIndices, GL_STATIC_DRAW); VERTEXBUFFER_H_CHECK_GL_ERROR

		Attribarray *walker = mAttribarray;
		while (walker)
		{
			glBindBuffer(GL_ARRAY_BUFFER, walker->mVBOHandle); VERTEXBUFFER_H_CHECK_GL_ERROR
			glBufferData(GL_ARRAY_BUFFER, walker->mFloatCount * sizeof(float), walker->mRawdata, GL_STATIC_DRAW); VERTEXBUFFER_H_CHECK_GL_ERROR
			walker = walker->mNext;
		}
		glBindVertexArray(0); VERTEXBUFFER_H_CHECK_GL_ERROR
	}
};

#endif // ST_VERTEXBUFFER_H_IMPLEMENTATION

#endif // ST_VERTEXBUFFER_H