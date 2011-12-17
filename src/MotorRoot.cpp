#include "MotorRoot.h"
#include "MotorRenderer.h"
#include "MotorTimer.h"
#include "MotorLogger.h"
#include <SDL/SDL.h>

namespace Motor {

	Root::Root() : renderer(new Renderer), timer(new Timer) {
	}
	
	Root::~Root() {
		delete timer;
		delete renderer;
	}

	int Root::initialize()
	{
		//TODO: Get window width/height from setting file
		if( SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0 ){
			Logger::getSingleton().log(Logger::CRITICALERROR, "SDL_Init failed");
			return 0;
		}
		surface = SDL_SetVideoMode(1024, 768, 32, SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL);
		if( surface == NULL ){
			Logger::getSingleton().log(Logger::CRITICALERROR, "SDL_SetVideoMode failed");
			return 0;
		}
		SDL_WM_SetCaption("MotorWindow", 0);
		if( !renderer->initialize(1024, 768) ) return 0;
		timer->initialize();
		return 1;
	}

	void Root::cleanup()
	{
		renderer->cleanup();
		SDL_FreeSurface(surface);
		SDL_Quit();
		return;
	}

	void Root::startRendering()
	{
		bool running = true;
		SDL_Event Event;
		//FPS counting
		int frameCount = 0;
		float frameTime = 0;
		timer->getElapsedTime(); //t=0
		while(running){
			while(SDL_PollEvent(&Event)) {
				switch( Event.type ){
				case SDL_KEYDOWN:
				case SDL_KEYUP:
					//mInputHandler->KeyDown( Event.key.keysym.sym , Event.key.state == SDL_PRESSED );
					break;
				case SDL_MOUSEMOTION:
					//mInputHandler->MouseMoved( Event.motion.x, Event.motion.y, Event.motion.xrel, Event.motion.yrel );
					//mRenderer->SetMousePosition( Event.motion.x, Event.motion.y );
					break;
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					//mInputHandler->MouseDown( Event.button.button, Event.button.state == SDL_PRESSED, Event.button.x, Event.button.y );
					break;
				case SDL_QUIT:
					running = false;
					break;
				default:
					break;
				}
			}
			float elapsedTime = timer->getElapsedTime();
			++frameCount;
			frameTime += elapsedTime;
			if( frameTime > 1.0f ){
				Logger::getSingleton().log(Logger::INFO, "FPS: %f (frametime %f)");
				//printf("Frametime: %f\t\tFPS: %f\n", frameTime/(float)frameCount, (float)frameCount/frameTime);
				frameCount = 0;
				frameTime = 0;
			}

			if( renderOneFrame() == false ) break;
		}
	}

	bool Root::renderOneFrame()
	{
		//Call frame listeners
		//for(iterator herp = derp)
		//	framelistener->preFrame(elapsedTime);
		renderer->renderFrame();
		SDL_GL_SwapBuffers();
		return true;
	}

}