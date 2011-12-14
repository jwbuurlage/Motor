#pragma once

//forward declarations to avoid unnecessary includes
struct SDL_Surface;

namespace Motor {

	class Renderer {
	public:
		Renderer();
		~Renderer();
		
		int initialize();
		void cleanup();

		bool renderFrame();

	private:
		SDL_Surface* surface;
	};

}