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

//forward declarations to avoid unnecessary includes
struct SDL_Surface;

namespace Motor {
	class Renderer;
	class Scene;
	class TextureManager;
	class MaterialManager;
	class MeshManager;
	class Timer;
	
	class Root : public Singleton<Root> {
	public:
		Root();
		~Root();
		
		int initialize();
		void cleanup();

		void startRendering(); //use with FrameListeners
		bool renderOneFrame();

		Scene* getScene(){ return currentScene; }
		Renderer* getRenderer(){ return renderer; }
	private:
		Timer* const timer;
		Renderer* const renderer;
		TextureManager* const textureManager;
		MaterialManager* const materialManager;
		MeshManager* const meshManager;
		Scene* currentScene;

		bool SDLinitialized;
		SDL_Surface* surface;
	};

}