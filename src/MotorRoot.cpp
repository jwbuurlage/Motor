#include "MotorRoot.h"
#include "MotorRenderer.h"
#include "MotorScene.h"
#include "MotorSceneObject.h"
#include "MotorCamera.h"
#include "MotorTextureManager.h"
#include "MotorMaterialManager.h"
#include "MotorMeshManager.h"
#include "MotorModelManager.h"
#include "MotorTimer.h"
#include "MotorLogger.h"
#include "MotorFilesystem.h"
#include <SFML/Window.hpp>
#include <sstream>

namespace Motor {

	template<> Root* Singleton<Root>::singleton = 0;

	Root::Root() : logger(new Logger),
					filesystem(new Filesystem),
					timer(new Timer),
					renderer(new Renderer),
					textureManager(new TextureManager),
					materialManager(new MaterialManager),
					meshManager(new MeshManager),
					modelManager(new ModelManager),
					currentScene(0), mouseX(0), mouseY(0), window(0) {
		initialized = false;
        running = false;
	}
	
	Root::~Root() {
		cleanup();
		delete timer;
		delete renderer;
		delete modelManager;
		delete meshManager;
		delete materialManager;
		delete textureManager;
		delete filesystem;
		delete logger;
	}

	int Root::initialize()
	{
		if( initialized ){
			Logger::getSingleton().log(Logger::WARNING, "Root object already initialized");
			return 1;
		}
		
		//TODO: Get window width/height from setting file
		if( !window ){
			sf::WindowSettings Settings;
			Settings.DepthBits         = 24; // Request a 24 bits depth buffer
			Settings.StencilBits       = 8;  // Request a 8 bits stencil buffer
			Settings.AntialiasingLevel = 2;  // Request 2 levels of antialiasing
			//window = new sf::Window(sf::VideoMode(1024, 768, 32), "OpenGL Window", sf::Style::Close, Settings);
            window = new sf::Window(sf::VideoMode::GetDesktopMode(), "Motor Project", sf::Style::Close, Settings);
		}

		if( !renderer->initialize(window->GetWidth(), window->GetHeight()) ) return 0;
		timer->initialize();

		textureManager->initialize();
		materialManager->initialize();
		meshManager->initialize();
		modelManager->initialize();

		currentScene = new Scene;
		addFrameListener(currentScene);
		currentScene->initialize();

		renderer->checkErrors();

		Logger::getSingleton().log(Logger::INFO, "Motor initialized");
		
		initialized = true;

		return 1;
	}

	void Root::cleanup()
	{
		inputListeners.clear();

		if( currentScene ){
			currentScene->cleanup();
			delete currentScene;
			currentScene = 0;
		}
		modelManager->cleanup();
		meshManager->cleanup();
		materialManager->cleanup();
		textureManager->cleanup();
		renderer->cleanup();
		if( window ){
			delete window;
			window = 0;
		}
		return;
	}

	void Root::startRendering()
	{
		if( initialized == false ){
			Logger::getSingleton().log(Logger::ERROR, "Trying to start render loop while motor is not initialized");
			return;
		}
		Logger::getSingleton().log(Logger::INFO, "Motor renderloop started");

        running = true;
		sf::Event Event;
		//FPS counting
		int frameCount = 0;
		float frameTime = 0;
		timer->getElapsedTime(); //t=0
		while(running && window->IsOpened() ){
			while(window->GetEvent(Event)) {
				switch( Event.Type ){
				case sf::Event::KeyPressed:
					keyDown( Event.Key.Code , true ); break;
				case sf::Event::KeyReleased:
					keyDown( Event.Key.Code , false ); break;
				case sf::Event::MouseMoved:
					mouseMoved( Event.MouseMove.X, Event.MouseMove.Y );
					//mRenderer->SetMousePosition( Event.motion.x, Event.motion.y );
					break;
				case sf::Event::MouseWheelMoved:
					mouseWheelMoved( Event.MouseWheel.Delta );
					break;
				case sf::Event::MouseButtonPressed:
					mouseDown( (MOUSEBUTTON)Event.MouseButton.Button, true, Event.MouseButton.X, Event.MouseButton.Y );
					break;
				case sf::Event::MouseButtonReleased:
					mouseDown( (MOUSEBUTTON)Event.MouseButton.Button, false, Event.MouseButton.X, Event.MouseButton.Y );
					break;
				case sf::Event::Closed:
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
            
            //update models
            ObjectContainer* objects = currentScene->getObjectList();
            for( ObjectIterator iter = objects->begin(); iter != objects->end(); ++iter ){
                (*iter)->update(elapsedTime);
            }

			//Call frame listeners
			for( std::vector<FrameListener*>::iterator it = frameListeners.begin(); it != frameListeners.end(); ++it ){
				(*it)->onFrame( elapsedTime );
			}

			if( renderOneFrame() == false ) break;
		}
	}
    
    void Root::stopRendering() {
        running = false;
    }


	bool Root::renderOneFrame()
	{
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
		window->Display();
		return renderer->checkErrors();
	}

	void Root::addInputListener(InputListener* listener)
	{
		inputListeners.push_back(listener);
	}

	void Root::removeInputListener(InputListener* listener)
	{
		for( std::vector<InputListener*>::iterator it = inputListeners.begin(); it != inputListeners.end(); ){
			if( *it == listener ) it = inputListeners.erase(it);
			else ++it;
		}
	}

	void Root::addFrameListener(FrameListener* listener)
	{
		frameListeners.push_back(listener);
	}

	void Root::removeFrameListener(FrameListener* listener)
	{
		for( std::vector<FrameListener*>::iterator it = frameListeners.begin(); it != frameListeners.end(); ){
			if( *it == listener ) it = frameListeners.erase(it);
			else ++it;
		}
	}

	void Root::keyDown(int key, bool KeyDown){
		for( std::vector<InputListener*>::iterator it = inputListeners.begin(); it != inputListeners.end(); ++it )
			if( (*it)->keyDown(key, KeyDown) == true ) break;

	}

	void Root::mouseDown(MOUSEBUTTON button, bool KeyDown, int x, int y){
		for( std::vector<InputListener*>::iterator it = inputListeners.begin(); it != inputListeners.end(); ++it )
			if( (*it)->mouseDown(button, KeyDown, x, y) == true ) break;
	}

	void Root::mouseWheelMoved(int delta){
		for( std::vector<InputListener*>::iterator it = inputListeners.begin(); it != inputListeners.end(); ++it )
			if( (*it)->mouseWheelMoved(delta) == true ) break;
	}

	void Root::mouseMoved(int x, int y){
		int dx = x - mouseX, dy = y - mouseY;
		mouseX = x; mouseY = y;
		for( std::vector<InputListener*>::iterator it = inputListeners.begin(); it != inputListeners.end(); ++it )
			if( (*it)->mouseMoved(x,y, dx, dy) == true ) break;
	}

}
