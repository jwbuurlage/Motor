#include "MotorRenderer.h"
#include "MotorLogger.h"
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
			Logger::getSingleton().log(Logger::CRITICALERROR, "SDL_SetVideoMode failed");
			return 0;
		}
		SDL_WM_SetCaption("TwinGame", 0);

		if( glewInit() != GLEW_OK ){
			Logger::getSingleton().log(Logger::CRITICALERROR, "glewInit failed");
			return 0;
		}
		if( !GLEW_VERSION_2_0 ){
			Logger::getSingleton().log(Logger::CRITICALERROR, "OpenGL 2.0 not supported");
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