#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "glad/include/glad/glad.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "/libraries/glm/glm/glm.hpp"
#include "/libraries/glm/glm/gtx/transform.hpp"
#include "/libraries/glm/glm/gtx/rotate_vector.hpp"
#include "/libraries/glm/glm/gtc/type_ptr.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

#define ST_H_IMPLEMENTATION
#include "../st_shader.h"
#include "../st_vertexbuffer.h"
#include "../st_renderbuf.h"
#include "../st_shadowmap.h"
#include "../st_texture.h"
#include "../st_generate_cube.h"
#include "../st_generate_sphere.h"
#include "../st_generate_torusknot.h"
#include "../st_generate_sprites.h"

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
Vertexbuffer *sphere;
Vertexbuffer *torusknot;
Vertexbuffer *sprites;
Shader tex, shadowpass, floory, knot, sprite;
GLuint glosstex, mosaictex, thegridtex, spritetex;
Renderbuf mirror;
Shadowmap shadowmap;

void init()
{
	shadowmap.init();
	mirror.init();
	glosstex = loadtexture("assets/gloss.png");
	mosaictex = loadtexture("assets/mosaic.png");
	thegridtex = loadtexture("assets/thegrid.png");
	spritetex = loadtexture("assets/sprite.png");
	sprites = generate_sprites(100);
	sphere = generate_sphere(200, 7);
	cube = generate_cube(150, 2, 150);
	torusknot = generate_torusknot(256, 7, 20, 0.1f, 0, 0, 0, 1, 64, 3, 5);
	tex.load("assets/tex.vs", "assets/tex.fs");
	knot.load("assets/knot.vs", "assets/knot.fs");
	shadowpass.load("assets/shadowpass.vs", "assets/shadowpass.fs");
	floory.load("assets/floory.vs", "assets/floory.fs");
	sprite.load("assets/sprite.vs", "assets/sprite.fs");

	float *wp = sprites->getAttribArrayPtr(2);
	float *sd = sprites->getAttribArrayPtr(3);
	int i;
	for (i = 0; i < 100; i++)
	{
		wp[i * 4 + 0] = sin(i * M_PI * 2 / 100) * 100;
		wp[i * 4 + 1] = sin(i * M_PI * 2 * 6 / 100) * 8 + 16;
		wp[i * 4 + 2] = cos(i * M_PI * 2 / 100) * 100;
		wp[i * 4 + 3] = 0;

		sd[i * 4 + 0] = 4;
		sd[i * 4 + 1] = 4;
		sd[i * 4 + 2] = 0;
		sd[i * 4 + 3] = 0.75;
	}

}

void draw_screen()
{
	int tick = SDL_GetTicks();
	glm::mat4 reflect = glm::mat4(
		1, 0, 0, 0,
		0, -1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
	glm::mat4 lookat = glm::lookAt(
		glm::vec3(sin(tick * 0.000345)*100.0f, sin(tick * 0.0001) * 20 + 35.0f, cos(tick * 0.000345)*100.0f),
		glm::vec3(0, 20, 0),
		glm::vec3(0, 1, 0));
	glm::mat4 proj = glm::perspective(90 * 3.14f / 360.0f, gScreenWidth / (float)gScreenHeight, 1.0f, 1000.0f);
	glm::mat4 translate_floory = glm::translate(glm::vec3(0, -1, 0));
	glm::mat4 translate_sphere = glm::translate(glm::vec3(0, 0, 0));
	glm::mat4 translate_torusknot = glm::translate(glm::vec3(0, 25, 0));
	glm::mat4 sphere_mvp = proj * lookat * translate_sphere;
	glm::mat4 torusknot_mvp = proj * lookat * translate_torusknot;
	glm::mat4 floory_mvp = proj * lookat * translate_floory;
	glm::mat4 sphere_mvp_m = proj * lookat * reflect * translate_sphere;
	glm::mat4 torusknot_mvp_m = proj * lookat * reflect * translate_torusknot;
	glm::mat4 translate_torusknot_m = reflect * translate_torusknot * reflect;
	glm::mat4 lookat_m = lookat * reflect;

	glm::vec4 lightpos = glm::vec4(sin(tick * 0.0007)*100.0f, sin(tick * 0.0001) * 20 + 35.0f, cos(tick * 0.00075)*100.0f, 1);
	/////////////////////////
	{
		shadowmap.enable();
		shadowpass.enable();
		shadowmap.shadowmatrix_proj(
			glm::vec3(lightpos.x, lightpos.y, lightpos.z),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0),
			(float)(90.0f * M_PI * 2.0f / 360.0f),
			1.0f, 1000.0f);
		glm::mat4 torusknot_mvp = shadowmap.mProjection * translate_torusknot * shadowmap.mModelview;

		glUniformMatrix4fv(shadowpass.getUniformLocation("mvp"), 1, 0, glm::value_ptr(torusknot_mvp));

		glCullFace(GL_BACK);
		torusknot->enable();
		torusknot->render();
		torusknot->disable();

		shadowpass.disable();
		shadowmap.disable();
	}
	/////////////////////////
	mirror.enable();
	{		
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		int tick = SDL_GetTicks();

		tex.enable();
		glUniformMatrix4fv(tex.getUniformLocation("mvp"), 1, 0, glm::value_ptr(sphere_mvp_m));
		glUniform1i(tex.getUniformLocation("tex"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, thegridtex);
		glCullFace(GL_BACK);
		sphere->enable();
		sphere->render();
		sphere->disable();
		tex.disable();

		knot.enable();
		glUniformMatrix4fv(knot.getUniformLocation("mvp"), 1, 0, glm::value_ptr(torusknot_mvp_m));
		glUniformMatrix4fv(knot.getUniformLocation("model"), 1, 0, glm::value_ptr(translate_torusknot_m));
		glUniformMatrix4fv(knot.getUniformLocation("shadowmatrix"), 1, 0, glm::value_ptr(shadowmap.mShadowmatrix));
		glUniform4fv(knot.getUniformLocation("lightpos"), 1, glm::value_ptr(lightpos));
		glUniform1i(knot.getUniformLocation("tex"), 0);
		glUniform1i(knot.getUniformLocation("shadowmap"), 3);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mosaictex);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, shadowmap.mShadowTex);
		glCullFace(GL_BACK);
		torusknot->enable();
		torusknot->render();
		torusknot->disable();
		knot.disable();

		sprite.enable();
		glUniformMatrix4fv(sprite.getUniformLocation("proj"), 1, 0, glm::value_ptr(proj));
		glUniformMatrix4fv(sprite.getUniformLocation("view"), 1, 0, glm::value_ptr(lookat_m));
		glUniform1i(sprite.getUniformLocation("tex"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, spritetex);
		sprites->enable();
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		sprites->render();
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		sprites->disable();
		sprite.disable();

	}
	mirror.disable();
	/////////////////////////

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	tex.enable();
	glUniformMatrix4fv(tex.getUniformLocation("mvp"), 1, 0, glm::value_ptr(sphere_mvp));
	glUniform1i(tex.getUniformLocation("tex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, thegridtex);
	glCullFace(GL_FRONT);
	sphere->enable();
	sphere->render();
	sphere->disable();
	tex.disable();

	knot.enable();
	glUniformMatrix4fv(knot.getUniformLocation("mvp"), 1, 0, glm::value_ptr(torusknot_mvp));
	glUniformMatrix4fv(knot.getUniformLocation("model"), 1, 0, glm::value_ptr(translate_torusknot));
	glUniformMatrix4fv(knot.getUniformLocation("shadowmatrix"), 1, 0, glm::value_ptr(shadowmap.mShadowmatrix));
	glUniform1i(knot.getUniformLocation("tex"), 0);
	glUniform1i(knot.getUniformLocation("shadowmap"), 3);
	glUniform4fv(knot.getUniformLocation("lightpos"), 1, glm::value_ptr(lightpos));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mosaictex);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, shadowmap.mShadowTex);
	glCullFace(GL_BACK);
	torusknot->enable();
	torusknot->render();
	torusknot->disable();

	knot.disable();

	floory.enable();
	glUniformMatrix4fv(floory.getUniformLocation("mvp"), 1, 0, glm::value_ptr(floory_mvp));
	glUniformMatrix4fv(floory.getUniformLocation("model"), 1, 0, glm::value_ptr(translate_floory));
	glUniformMatrix4fv(floory.getUniformLocation("shadowmatrix"), 1, 0, glm::value_ptr(shadowmap.mShadowmatrix));
	glUniform1i(floory.getUniformLocation("tex"), 0);
	glUniform1i(floory.getUniformLocation("gloss"), 1);
	glUniform1i(floory.getUniformLocation("mirror"), 2);
	glUniform1i(floory.getUniformLocation("shadowmap"), 3);
	glUniform2f(floory.getUniformLocation("viewport"), (float)gScreenWidth, (float)gScreenHeight);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mosaictex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, glosstex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mirror.mRenderTex);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, shadowmap.mShadowTex);
	glCullFace(GL_BACK);

	cube->enable();
	cube->render();
	cube->disable();
	floory.disable();

	sprite.enable();
	glUniformMatrix4fv(sprite.getUniformLocation("proj"), 1, 0, glm::value_ptr(proj));
	glUniformMatrix4fv(sprite.getUniformLocation("view"), 1, 0, glm::value_ptr(lookat));
	glUniform1i(sprite.getUniformLocation("tex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, spritetex);
	sprites->enable();
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	sprites->render();
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	sprites->disable();
	sprite.disable();

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
