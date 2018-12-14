// by Jari Komppa 2018
// Under unlicense. Google it.
// Use ST_RENDERBUF_H_IMPLEMENTATION in one source file before including this.
// (Or if you're using a bunch of ST_ headers and want to implement all, just use ST_H_IMPLEMENTATION)
// Needs opengl headers before it, naturally.

#ifndef ST_RENDERBUF_H
#define ST_RENDERBUF_H

namespace st
{

	class Renderbuf
	{
	public:
		GLuint mRenderFBO, mRenderRB, mRenderTex;
		GLint mPrevFB;
		GLint mPrevViewport[4];
		int mRenderTextureDimension;

		void init(int aRenderTextureDimension = 1024);
		void enable();
		void disable();
		virtual ~Renderbuf();
		void check_gl_error(const char * fn, int l);
		Renderbuf();
	};
}

#if defined(ST_RENDERBUF_H_IMPLEMENTATION) || defined(ST_H_IMPLEMENTATION)

#define RENDERBUF_H_CHECK_GL_ERROR check_gl_error(__FILE__,__LINE__);

namespace st
{
	void Renderbuf::check_gl_error(const char * fn, int l)
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

	Renderbuf::Renderbuf()
	{
		mRenderTex = 0;
		mRenderFBO = 0;
		mRenderRB = 0;
	}

	Renderbuf::~Renderbuf()
	{
		if (mRenderTex)
		{
			glDeleteTextures(1, &mRenderTex); RENDERBUF_H_CHECK_GL_ERROR
		}
		if (mRenderFBO)
		{
			glDeleteFramebuffers(1, &mRenderFBO); RENDERBUF_H_CHECK_GL_ERROR
		}
		if (mRenderRB)
		{
			glDeleteRenderbuffers(1, &mRenderRB); RENDERBUF_H_CHECK_GL_ERROR
		}
	}

	void Renderbuf::init(int aRenderTextureDimension)
	{
		mRenderTextureDimension = aRenderTextureDimension;

		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &mPrevFB); RENDERBUF_H_CHECK_GL_ERROR
			
		glActiveTexture(GL_TEXTURE0); RENDERBUF_H_CHECK_GL_ERROR
		glGenTextures(1, &mRenderTex); RENDERBUF_H_CHECK_GL_ERROR
		glBindTexture(GL_TEXTURE_2D, mRenderTex); RENDERBUF_H_CHECK_GL_ERROR
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); RENDERBUF_H_CHECK_GL_ERROR
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); RENDERBUF_H_CHECK_GL_ERROR

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); RENDERBUF_H_CHECK_GL_ERROR//GL_CLAMP_TO_BORDER);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); RENDERBUF_H_CHECK_GL_ERROR// GL_CLAMP_TO_BORDER);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, aRenderTextureDimension, aRenderTextureDimension, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0); RENDERBUF_H_CHECK_GL_ERROR

		glGenFramebuffers(1, &mRenderFBO); RENDERBUF_H_CHECK_GL_ERROR
		glBindFramebuffer(GL_FRAMEBUFFER, mRenderFBO); RENDERBUF_H_CHECK_GL_ERROR

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mRenderTex, 0); RENDERBUF_H_CHECK_GL_ERROR

		glGenRenderbuffers(1, &mRenderRB); RENDERBUF_H_CHECK_GL_ERROR
		glBindRenderbuffer(GL_RENDERBUFFER, mRenderRB); RENDERBUF_H_CHECK_GL_ERROR
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, aRenderTextureDimension, aRenderTextureDimension); RENDERBUF_H_CHECK_GL_ERROR

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); RENDERBUF_H_CHECK_GL_ERROR
		
		// .. do something with the status maybe?

		glBindFramebuffer(GL_FRAMEBUFFER, mPrevFB); RENDERBUF_H_CHECK_GL_ERROR
	}

	void Renderbuf::enable()
	{
		glGetIntegerv(GL_VIEWPORT, &mPrevViewport[0]); RENDERBUF_H_CHECK_GL_ERROR
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &mPrevFB); RENDERBUF_H_CHECK_GL_ERROR
		glBindFramebuffer(GL_FRAMEBUFFER, mRenderFBO); RENDERBUF_H_CHECK_GL_ERROR
		glViewport(0, 0, mRenderTextureDimension, mRenderTextureDimension); RENDERBUF_H_CHECK_GL_ERROR
	}

	void Renderbuf::disable()
	{
		glViewport(mPrevViewport[0], mPrevViewport[1], mPrevViewport[2], mPrevViewport[3]); RENDERBUF_H_CHECK_GL_ERROR
		glBindFramebuffer(GL_FRAMEBUFFER, mPrevFB); RENDERBUF_H_CHECK_GL_ERROR
	}
}

#endif // implementation
#endif // include guard