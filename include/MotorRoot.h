/* This document is part of the Motor project,
 * an experimental graphics engine.
 *
 *	Jan-Willem Buurlage, 	j.buurlage@students.uu.nl. 
 *	Tom Bannink, 			t.bannink@students.uu.nl
 *
 * http://www.github.com/jwbuurlage/Motor 
 * 
 */
#pragma once
#include "MotorSingleton.h"
#include <vector>

//forward declarations to avoid unnecessary includes
struct SDL_Surface;
namespace sf{
	class Window;
};

namespace Motor {
	class Renderer;
	class Scene;
	class TextureManager;
	class MaterialManager;
	class MeshManager;
	class ModelManager;
	class Timer;
	class Logger;
	class Filesystem;

	//These are the constants as SFML uses them, do not change
	typedef enum { BUTTON_LEFT = 0, BUTTON_RIGHT, BUTTON_MIDDLE, XButton1, XButton2 } MOUSEBUTTON;

	//Subclass this to receive input
	class InputListener{
	public:
		//When returning false, next listener will be called. When returning true, the chain stops.
		virtual bool keyDown(int key, bool keyDown){ return false; }
		virtual bool mouseDown(MOUSEBUTTON button, bool buttonDown, int x, int y){ return false; }
		virtual bool mouseWheelMoved(int delta){ return false; }
		virtual bool mouseMoved(int x, int y, int dx, int dy){ return false; }
	};

	//Subclass to be called every frame
	class FrameListener{
	public:
		//Time is since previous frame
		virtual void onFrame(float elapsedTime){ return; }
	};
	
	class Root : public Singleton<Root> {
	public:
		Root();
		~Root();
		
		int initialize();
		void cleanup();
        
		void startRendering(); //use with FrameListeners
        void stopRendering(); 
		bool renderOneFrame();

		//TODO: Extra arguments like APPEND_LAST or FRONT or CALL_ALWAYS or something??
		void addInputListener(InputListener* listener);
		void removeInputListener(InputListener* listener);

		void addFrameListener(FrameListener* listener);
		void removeFrameListener(FrameListener* listener);

		Scene* getScene(){ return currentScene; }
		Renderer* getRenderer(){ return renderer; }
	private:
        bool running;
        
		Logger* const logger;
		Filesystem* const filesystem;
		Timer* const timer;
		Renderer* const renderer;
		TextureManager* const textureManager;
		MaterialManager* const materialManager;
		MeshManager* const meshManager;
		ModelManager* const modelManager;
		Scene* currentScene;

		std::vector<FrameListener*> frameListeners;
		std::vector<InputListener*> inputListeners;
		void keyDown(int key, bool KeyDown);
		void mouseDown(MOUSEBUTTON button, bool KeyDown, int x, int y);
		void mouseWheelMoved(int delta);
		void mouseMoved(int x, int y);
		int mouseX, mouseY; //To supply relative movement
		
		sf::Window* window;
	};

}