// by Jari Komppa 2018
// Under unlicense. Google it.
// Use ST_SHADER_H_IMPLEMENTATION in one source file before including this.
// (Or if you're using a bunch of ST_ headers and want to implement all, just use ST_H_IMPLEMENTATION)
// Needs opengl headers before it, naturally.

#ifndef ST_SHADER_H
#define ST_SHADER_H

namespace st
{

	class Shader
	{
	public:
		int mVtxshader;
		int mFragshader;
		int mProgram;

		int getUniformLocation(const char * aName);
		int getAttributeLocation(const char * aName);
		Shader();
		void load(char * vtxshaderfilename, char * pixshaderfilename);
		void loadmem(char * vtxshader, char * pixshader);
		void enable();
		void disable();
		void loadshader(int shaderid, char * filename);
		void loadshadermem(int shaderid, char *membuf, char *name);
		void check_gl_error(const char * fn, int l);
	};
}

#if defined(ST_SHADER_H_IMPLEMENTATION) || defined(ST_H_IMPLEMENTATION)

namespace st
{

#define SHADER_H_CHECK_GL_ERROR check_gl_error(__FILE__,__LINE__);

	void Shader::check_gl_error(const char * fn, int l)
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

	int Shader::getUniformLocation(const char * aName)
	{
		int v = glGetUniformLocation(mProgram, aName); SHADER_H_CHECK_GL_ERROR
		if (v < 0)
		{
			char tmp[256];
			sprintf_s(tmp, 256, "%s::%d:%s", __FILE__, __LINE__, aName);
			MessageBoxA(NULL, "Bad uniform name", tmp, MB_OK);
		}
		return v;
	}

	int Shader::getAttributeLocation(const char * aName)
	{
		int ret = glGetAttribLocation(mProgram, aName); SHADER_H_CHECK_GL_ERROR
		if (ret < 0)
		{
			char tmp[256];
			sprintf_s(tmp, 256, "%s::%d:%s", __FILE__, __LINE__, aName);
			MessageBoxA(NULL, "Bad attribute name", tmp, MB_OK);
		}
		return ret;
	}

	Shader::Shader()
	{
		mVtxshader = 0;
		mFragshader = 0;
		mProgram = 0;
	}

	void Shader::load(char * vtxshaderfilename, char * pixshaderfilename)
	{
		if (vtxshaderfilename)
		{
			mVtxshader = glCreateShader(GL_VERTEX_SHADER); SHADER_H_CHECK_GL_ERROR
			loadshader(mVtxshader, vtxshaderfilename); SHADER_H_CHECK_GL_ERROR
		}
		if (pixshaderfilename)
		{
			mFragshader = glCreateShader(GL_FRAGMENT_SHADER); SHADER_H_CHECK_GL_ERROR
			loadshader(mFragshader, pixshaderfilename); SHADER_H_CHECK_GL_ERROR
		}
		mProgram = glCreateProgram(); SHADER_H_CHECK_GL_ERROR
		if (mFragshader)
		{ 
			glAttachShader(mProgram, mFragshader); SHADER_H_CHECK_GL_ERROR
		}
		if (mVtxshader)
		{
			glAttachShader(mProgram, mVtxshader); SHADER_H_CHECK_GL_ERROR
		}
		glLinkProgram(mProgram); SHADER_H_CHECK_GL_ERROR
		
		int res;
		glGetProgramiv(mProgram, GL_LINK_STATUS, &res); SHADER_H_CHECK_GL_ERROR
		if (res != GL_TRUE)
		{
			char temp[8192];
			int len;
			glGetProgramInfoLog(mProgram, 8192, &len, temp);
			MessageBoxA(NULL, temp, "Shader link failed", MB_OK);
			static int retrycount = 0;
			if (retrycount > 5)
				exit(-1);
			load(vtxshaderfilename, pixshaderfilename);
			retrycount = 0;
		}
	}

	void Shader::loadmem(char * vtxshader, char * pixshader)
	{
		if (vtxshader)
		{
			mVtxshader = glCreateShader(GL_VERTEX_SHADER); SHADER_H_CHECK_GL_ERROR
			loadshadermem(mVtxshader, vtxshader, (char*)"vtxshader"); SHADER_H_CHECK_GL_ERROR
		}
		if (pixshader)
		{
			mFragshader = glCreateShader(GL_FRAGMENT_SHADER); SHADER_H_CHECK_GL_ERROR
			loadshadermem(mFragshader, pixshader, (char*)"fragshader"); SHADER_H_CHECK_GL_ERROR
		}
		mProgram = glCreateProgram(); SHADER_H_CHECK_GL_ERROR
		if (mFragshader)
		{ 
			glAttachShader(mProgram, mFragshader); SHADER_H_CHECK_GL_ERROR
		}
		if (mVtxshader)
		{
			glAttachShader(mProgram, mVtxshader); SHADER_H_CHECK_GL_ERROR
		}
		glLinkProgram(mProgram); SHADER_H_CHECK_GL_ERROR
		
		int res;
		glGetProgramiv(mProgram, GL_LINK_STATUS, &res); SHADER_H_CHECK_GL_ERROR
		if (res != GL_TRUE)
		{
			char temp[8192];
			int len;
			glGetProgramInfoLog(mProgram, 8192, &len, temp);
			MessageBoxA(NULL, temp, "Shader link failed", MB_OK);
		}
	}

	void Shader::enable()
	{
		glUseProgram(mProgram); SHADER_H_CHECK_GL_ERROR
	}

	void Shader::disable()
	{
		glUseProgram(0); SHADER_H_CHECK_GL_ERROR
	}

	void Shader::loadshadermem(int shaderid, char *membuf, char *name) 
	{
		char * lines[1024]; // 1k lines should be enough for everybody(tm)
		int lengths[1024];
		int linecount = 0;
		int i = 0;
		lines[linecount] = membuf;
		linecount++;
		while (membuf[i] != 0)
		{
			// This is a bit hacky. I don't like it.
			if (membuf[i] == '\r')
			{
				membuf[i] = 0;
				lines[linecount] = membuf + i + 1;
				linecount++;
			}
			i++;
		}
		for (i = 0; i < linecount; i++)
			lengths[i] = (int)strlen(lines[i]);
		glShaderSource(shaderid, linecount, (const GLchar**)lines, lengths); SHADER_H_CHECK_GL_ERROR
		glCompileShader(shaderid); SHADER_H_CHECK_GL_ERROR
		int res;
		glGetShaderiv(shaderid, GL_COMPILE_STATUS, &res); SHADER_H_CHECK_GL_ERROR
		if (res != GL_TRUE)
		{
			char temp[8192];
			int len;
			glGetShaderInfoLog(shaderid, 8192, &len, temp);
			MessageBoxA(NULL, temp, name, MB_OK);
		}
	}

	void Shader::loadshader(int shaderid, char * filename)
	{
		int len;
		char * buf;
		FILE * f;
		fopen_s(&f, filename, "rb");
		if (f == NULL)
			return;
		fseek(f, 0, SEEK_END);
		len = ftell(f);
		fseek(f, 0, SEEK_SET);
		buf = new char[len + 1];
		memset(buf, 0, len + 1);
		fread(buf, 1, len, f);
		fclose(f);
		loadshadermem(shaderid, buf, filename);
		delete[] buf;
	}
}
#endif // ST_SHADER_H_IMPLEMENTATION

#endif // ST_SHADER_H