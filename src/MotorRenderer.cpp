#include "MotorRenderer.h"
#include <SDL/SDL.h>
#include <GL/glew.h>

namespace Motor {
	
	Renderer::Renderer(){
	}

	Renderer::~Renderer(){
	}

	int Renderer::initialize(){
		surface = SDL_SetVideoMode(1024, 768, 32, SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL);
		if( surface == NULL ){
			return 0;
		}
		SDL_WM_SetCaption("TwinGame", 0);

		if( glewInit() != GLEW_OK ){
			//setError "glewInit failed\n"
			return 0;
		}
		if( !GLEW_VERSION_2_0 ){
			//setError "OpenGL 2.0 not supported\n"
			return 0;
		}

		return 1;
	}

	void Renderer::cleanup(){
		SDL_FreeSurface(surface);
		return;
	}

	bool Renderer::renderFrame(){
		glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		SDL_GL_SwapBuffers();
		return true;
	}

}