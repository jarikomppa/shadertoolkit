// by Jari Komppa 2018
// Under unlicense. Google it.
// Use ST_RENDERBUF_H_IMPLEMENTATION in one source file before including this.
// (Or if you're using a bunch of ST_ headers and want to implement all, just use ST_H_IMPLEMENTATION)
// Needs opengl headers before it, naturally.

// This one depends on glm, since it needs to calculate them matrices.

#ifndef ST_SHADOWMAP_H
#define ST_SHADOWMAP_H

namespace st
{
	class Shadowmap
	{
	public:
		glm::mat4 mShadowmatrix;
		glm::mat4 mMVP;
		glm::mat4 mModelview;
		glm::mat4 mProjection;
		glm::vec4 mLightpos;
		GLint mPrevFB;
		GLint mPrevViewport[4];
		GLuint mShadowFBO, mShadowTex;
		int mShadowTextureDimension;
		Shadowmap();
		virtual ~Shadowmap();
		void init(int aShadowTextureDimension = 1024);
		void shadowmatrix_proj(const glm::vec3 &aLightPos, const glm::vec3 &aLightAt, const glm::vec3 &aLightUp, float aLightFov, float znear, float zfar);
		void shadowmatrix_ortho(const glm::vec3 &aLightPos, const glm::vec3 &aLightAt, const glm::vec3 &aLightUp, float left, float right, float bottom, float top, float znear, float zfar);
		void shadowmatrix(const glm::vec3 &aLightPos, const glm::vec3 &aLightAt, const glm::vec3 &aLightUp, const glm::mat4 &projection);
		void enable();
		void disable();
		void check_gl_error(const char * fn, int l);
	};

}

#if defined(ST_SHADOWMAP_H_IMPLEMENTATION) || defined(ST_H_IMPLEMENTATION)

#define SHADOWMAP_H_CHECK_GL_ERROR check_gl_error(__FILE__,__LINE__);

namespace st
{
	void Shadowmap::check_gl_error(const char * fn, int l)
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


	Shadowmap::Shadowmap()
	{
		mShadowTex = 0;
		mShadowFBO = 0;
	}

	Shadowmap::~Shadowmap()
	{
		if (mShadowTex)
		{
			glDeleteTextures(1, &mShadowTex); SHADOWMAP_H_CHECK_GL_ERROR
		}
		if (mShadowFBO)
		{
			glDeleteFramebuffers(1, &mShadowFBO); SHADOWMAP_H_CHECK_GL_ERROR
		}
	}

	void Shadowmap::init(int aShadowTextureDimension)
	{
		mShadowTextureDimension = aShadowTextureDimension;

		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &mPrevFB); SHADOWMAP_H_CHECK_GL_ERROR

		glGenFramebuffers(1, &mShadowFBO); SHADOWMAP_H_CHECK_GL_ERROR
		glBindFramebuffer(GL_FRAMEBUFFER, mShadowFBO); SHADOWMAP_H_CHECK_GL_ERROR

		glActiveTexture(GL_TEXTURE0); SHADOWMAP_H_CHECK_GL_ERROR
		glGenTextures(1, &mShadowTex); SHADOWMAP_H_CHECK_GL_ERROR
		glBindTexture(GL_TEXTURE_2D, mShadowTex); SHADOWMAP_H_CHECK_GL_ERROR

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, aShadowTextureDimension, aShadowTextureDimension, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0); SHADOWMAP_H_CHECK_GL_ERROR

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); SHADOWMAP_H_CHECK_GL_ERROR
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); SHADOWMAP_H_CHECK_GL_ERROR

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); SHADOWMAP_H_CHECK_GL_ERROR
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); SHADOWMAP_H_CHECK_GL_ERROR

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mShadowTex, 0); SHADOWMAP_H_CHECK_GL_ERROR

		glDrawBuffer(GL_NONE); SHADOWMAP_H_CHECK_GL_ERROR
		glReadBuffer(GL_NONE); SHADOWMAP_H_CHECK_GL_ERROR

		glBindFramebuffer(GL_FRAMEBUFFER, mPrevFB); SHADOWMAP_H_CHECK_GL_ERROR
	}

	void Shadowmap::shadowmatrix_proj(const glm::vec3 &aLightPos, const glm::vec3 &aLightAt, const glm::vec3 &aLightUp, float aLightFov, float znear, float zfar)
	{
		glm::mat4 projection = glm::perspective(aLightFov, 1.0f, znear, zfar);
		shadowmatrix(aLightPos, aLightAt, aLightUp, projection);
	}

	void Shadowmap::shadowmatrix_ortho(const glm::vec3 &aLightPos, const glm::vec3 &aLightAt, const glm::vec3 &aLightUp, float left, float right, float bottom, float top, float znear, float zfar)
	{
		glm::mat4 projection = glm::ortho<float>(left, right, bottom, top, znear, zfar);
		shadowmatrix(aLightPos, aLightAt, aLightUp, projection);
	}

	void Shadowmap::shadowmatrix(const glm::vec3 &aLightPos, const glm::vec3 &aLightAt, const glm::vec3 &aLightUp, const glm::mat4 &projection)
	{
		mProjection = projection;
		mLightpos.x = aLightPos.x;
		mLightpos.y = aLightPos.y;
		mLightpos.z = aLightPos.z;
		mLightpos.w = 1;
		mModelview = glm::lookAt(aLightPos, aLightAt, aLightUp);

		glm::mat4 bias = {
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0 };

		mShadowmatrix = bias;
		mShadowmatrix *= mProjection;
		mShadowmatrix *= mModelview;

		mMVP = projection * mModelview;
	}

	void Shadowmap::enable()
	{
		glGetIntegerv(GL_VIEWPORT, &mPrevViewport[0]); SHADOWMAP_H_CHECK_GL_ERROR
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &mPrevFB); SHADOWMAP_H_CHECK_GL_ERROR

		glBindFramebuffer(GL_FRAMEBUFFER, mShadowFBO); SHADOWMAP_H_CHECK_GL_ERROR
		glViewport(0, 0, mShadowTextureDimension, mShadowTextureDimension); SHADOWMAP_H_CHECK_GL_ERROR

		glClearDepth(1); SHADOWMAP_H_CHECK_GL_ERROR
		glClear(GL_DEPTH_BUFFER_BIT); SHADOWMAP_H_CHECK_GL_ERROR
	}

	void Shadowmap::disable()
	{
		glViewport(mPrevViewport[0], mPrevViewport[1], mPrevViewport[2], mPrevViewport[3]); SHADOWMAP_H_CHECK_GL_ERROR
		glBindFramebuffer(GL_FRAMEBUFFER, mPrevFB); SHADOWMAP_H_CHECK_GL_ERROR
	}
}

#endif // implementation
#endif // include guard