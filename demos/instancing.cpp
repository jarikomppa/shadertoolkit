#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "glad/include/glad/glad.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "/libraries/glm/glm/glm.hpp"
#include "/libraries/glm/glm/gtx/transform.hpp"
#include "/libraries/glm/glm/gtx/rotate_vector.hpp"
#include "/libraries/glm/glm/gtc/type_ptr.hpp"

#define ST_SHADER_H_IMPLEMENTATION
#include "../st_shader.h"

#define ST_VERTEXBUFFER_H_IMPLEMENTATION
#include "../st_vertexbuffer.h"

#define ST_GENERATE_CUBE_H_IMPLEMENTATION
#include "../st_generate_cube.h"

using namespace st;

#define TITLE "ShaderToolkit Demo"
// For windowed mode
#define DESIRED_WINDOW_WIDTH (1920/2)
// For windowed mode
#define DESIRED_WINDOW_HEIGHT (1080/2)
// Desired aspect ratio. 640x480 = 4/3. Comment out to use whatever the desktop is set.
#define DESIRED_ASPECT ((float)(DESIRED_WINDOW_WIDTH)/(float)(DESIRED_WINDOW_HEIGHT))

//#define FULLSCREEN_BY_DEFAULT

int gScreenWidth = 0;
int gScreenHeight = 0;
SDL_Window *gSDLWindow;

void initvideo(int argc)
{
	int bpp = 0;
	int flags = 0;
	SDL_Rect info;
	SDL_GetDisplayBounds(0, &info);

#ifdef _DEBUG
	int fsflag = 0;
#else
#ifdef FULLSCREEN_BY_DEFAULT
	int fsflag = 1;
#else
	int fsflag = 0;
#endif
#endif

	if (argc > 1) fsflag = !fsflag;

	if (fsflag)
	{
		SDL_ShowCursor(0); // hide cursor in full screen
		gScreenWidth = info.w;
		gScreenHeight = info.h;
		flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN;
	}
	else
	{
		gScreenWidth = DESIRED_WINDOW_WIDTH;
		gScreenHeight = DESIRED_WINDOW_HEIGHT;
		flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	gSDLWindow = SDL_CreateWindow(
		"",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		gScreenWidth,
		gScreenHeight,
		flags);

	if (gSDLWindow == 0)
	{
		fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
		SDL_Quit();
		exit(0);
	}

	SDL_GLContext glcontext = SDL_GL_CreateContext(gSDLWindow);

	SDL_GL_SetSwapInterval(1);

	if (!gladLoadGL()) {
		printf("Something went wrong!\n");
		exit(-1);
	}

#ifdef DESIRED_ASPECT
	float aspect = DESIRED_ASPECT;
	if (((float)gScreenWidth / gScreenHeight) > aspect)
	{
		float realx = gScreenHeight * aspect;
		float extrax = gScreenWidth - realx;

		glViewport((GLsizei)(extrax / 2), 0, (GLsizei)realx, (GLsizei)gScreenHeight);
	}
	else
	{
		float realy = gScreenWidth / aspect;
		float extray = gScreenHeight - realy;

		glViewport(0, (GLsizei)(extray / 2), (GLsizei)gScreenWidth, (GLsizei)realy);
	}
#else
	glViewport(0, 0, (GLsizei)gScreenWidth, (GLsizei)gScreenHeight);
#endif
}

void process_events()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
//			handle_key(event.key.keysym.sym, 1);
			break;
		case SDL_KEYUP:
			exit(0);
//			handle_key(event.key.keysym.sym, 0);
			break;
		case SDL_QUIT:
			SDL_Quit();
			exit(0);
			break;
		case SDL_WINDOWEVENT_RESIZED:
			gScreenWidth = event.window.data1;
			gScreenHeight = event.window.data2;
			initvideo(0);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

Vertexbuffer *cube;
Shader flat;

void init()
{
	cube = generate_cube(10, 10, 10);
	cube->mInstanceCount = 5 * 5 * 5;
	float * inst = cube->addAttribArray(3, 4, 1);

	int i, j, k;
	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 5; j++)
		{
			for (k = 0; k < 5; k++)
			{
				inst[(k + j * 5 + i * 5 * 5) * 3 + 0] = (i - 2) * 25.0f;
				inst[(k + j * 5 + i * 5 * 5) * 3 + 1] = (j - 2) * 25.0f;
				inst[(k + j * 5 + i * 5 * 5) * 3 + 2] = (k - 2) * 25.0f;
			}
		}
	}

	flat.loadmem((char*)
	"#version 330\n"
	"#extension GL_ARB_explicit_uniform_location : enable\n"
	"\n"
	"layout(location = 1) in vec4 vertexposition;\n"
	"layout(location = 4) in vec4 instanceposition;\n"
	"\n"
	"uniform mat4 mvp;\n"
	"uniform vec4 incolor;\n"
	"\n"
	"out vec4 color;\n"
	"out vec4 coord;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	gl_Position = mvp * (vertexposition + vec4(instanceposition.xyz,0));\n"
	"	color = incolor;\n"
	"	coord = normalize(vertexposition);\n"
	"}\n",
	(char*)
	"#version 330\n"
	"#extension GL_ARB_explicit_uniform_location : enable\n"
	"\n"
	"in vec4 coord;\n"
	"in vec4 color;\n"
	"out vec4 fragcolor;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	//fragcolor = color;\n"
	"	fragcolor = color + coord;\n"
	"}\n");
}

void draw_screen()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	int tick = SDL_GetTicks();
	glm::mat4 lookat = glm::lookAt(glm::vec3(sin(tick * 0.000345)*200.0f, 50, cos(tick * 0.000345)*200.0f), glm::vec3(0, -10, 0), glm::vec3(0, 1, 0));
	glm::mat4 proj = glm::perspective(90 * 3.14f / 360.0f, gScreenWidth / (float)gScreenHeight, 1.0f, 1000.0f);
	glm::mat4 mvp = proj * lookat;

	flat.enable();
	glUniformMatrix4fv(flat.getUniformLocation("mvp"), 1, 0, glm::value_ptr(mvp));
	glm::vec4 col = glm::vec4(sin(tick * 0.00123), sin(tick * 0.00135), sin(tick * 0.00146), 1);
	glUniform4fv(flat.getUniformLocation("incolor"), 1, glm::value_ptr(col));

	cube->enable();
	cube->render();
	cube->disable();
	flat.disable();
	SDL_Delay(1);
	SDL_GL_SwapWindow(gSDLWindow);
}

//////////////////////////////////////////////////////////////////////////

int main(int argc, char** args)
{

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0)
	{
		fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
		SDL_Quit();
		exit(0);
	}

	initvideo(argc);

	// set window title
	SDL_SetWindowTitle(gSDLWindow, TITLE " - http://iki.fi/sol/");

	init();

	while (1)
	{
		process_events();
		draw_screen();
	}

	return 0;
}
