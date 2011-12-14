#include "MotorRoot.h"
#include "MotorRenderer.h"
#include <SDL/SDL.h>

namespace Motor {

	Root::Root() : renderer(new Renderer) {
	}
	
	Root::~Root() {
		delete renderer;
	}

	int Root::initialize()
	{
		if( SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0 ) return 0;
		if( !renderer->initialize() ) return 0;
		return 1;
	}

	void Root::cleanup()
	{
		renderer->cleanup();
		SDL_Quit();
		return;
	}

}