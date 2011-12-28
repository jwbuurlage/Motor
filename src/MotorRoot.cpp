#include "MotorRoot.h"
#include "MotorRenderer.h"
#include "MotorScene.h"
#include "MotorCamera.h"
#include "MotorTextureManager.h"
#include "MotorMaterialManager.h"
#include "MotorMeshManager.h"
#include "MotorTimer.h"
#include "MotorLogger.h"
#include <SDL/SDL.h>
#include <sstream>

namespace Motor {

	template<> Root* Singleton<Root>::singleton = 0;

	Root::Root() : timer(new Timer),
					renderer(new Renderer),
					textureManager(new TextureManager),
					materialManager(new MaterialManager),
					meshManager(new MeshManager),
					currentScene(0) {
		SDLinitialized = false;
	}
	
	Root::~Root() {
		cleanup();
		delete timer;
		delete renderer;
		delete materialManager;
		delete textureManager;
	}

	int Root::initialize()
	{
		//TODO: Get window width/height from setting file
		if( SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0 ){
			Logger::getSingleton().log(Logger::CRITICALERROR, "SDL_Init failed");
			return 0;
		}
		SDLinitialized = true;
		surface = SDL_SetVideoMode(1024, 768, 32, SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL);
		if( surface == NULL ){
			Logger::getSingleton().log(Logger::CRITICALERROR, "SDL_SetVideoMode failed");
			return 0;
		}
		SDL_WM_SetCaption("MotorWindow", 0);

		if( !renderer->initialize(1024, 768) ) return 0;
		timer->initialize();

		textureManager->initialize();
		materialManager->initialize();
		meshManager->initialize();

		currentScene = new Scene;
		currentScene->initialize();

		renderer->checkErrors();

		return 1;
	}

	void Root::cleanup()
	{
		if( currentScene ){
			currentScene->cleanup();
			delete currentScene;
			currentScene = 0;
		}
		meshManager->cleanup();
		materialManager->cleanup();
		textureManager->cleanup();
		renderer->cleanup();
		if( surface ){
			SDL_FreeSurface(surface);
			surface = 0;
		}
		if( SDLinitialized ){
			SDL_Quit();
			SDLinitialized = false;
		}
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
				std::stringstream text;
				text << "FPS: " << (float)frameCount/frameTime << "\t(frametime " << frameTime/(float)frameCount << ")";
				Logger::getSingleton().log(Logger::INFO, text.str().c_str() );
				frameCount = 0;
				frameTime = 0;
			}

			//TODO: Add scene as a framelistener!
			currentScene->update(elapsedTime);

			if( renderOneFrame() == false ) break;
		}
	}

	bool Root::renderOneFrame()
	{
		//Call frame listeners
		//for(iterator herp = derp)
		//	framelistener->preFrame(elapsedTime);

		//Do this every frame because currentScene might switch
		if( currentScene ){
			renderer->setObjectList(currentScene->getObjectList());
			renderer->setEffectList(currentScene->getEffectList());
			renderer->setLightList(currentScene->getLightList());
			currentScene->getCamera()->updateViewMatrix(renderer->viewMatrixPtr());
		}else{
			renderer->setObjectList(0);
			renderer->setEffectList(0);
			renderer->setLightList(0);
		}

		renderer->renderFrame();
		SDL_GL_SwapBuffers();
		return renderer->checkErrors();
	}

}
