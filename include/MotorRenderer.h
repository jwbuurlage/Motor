#pragma once

namespace Motor {

	class Renderer {
	public:
		Renderer();
		~Renderer();
		
		int initialize(int width, int height);
		void cleanup();

		void setWindowSize(int width, int height);

		bool renderFrame();

	private:
		int windowWidth, windowHeight;
	};

}