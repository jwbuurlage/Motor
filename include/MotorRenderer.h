#pragma once
#include "matrix.h"

namespace Motor {
	class ShaderManager;

	class Renderer {
	public:
		Renderer();
		~Renderer();
		
		int initialize(int width, int height);
		void cleanup();

		void setWindowSize(int width, int height);

		bool renderFrame();

	private:
		bool initialized;
		int windowWidth, windowHeight;
		ShaderManager* shaderManager;

		//TODO: the application should decide what shaders to load
		//Expose ShaderManager in a nice way to application
		void loadShaders();

		//Matrices that are the same for all frames are saved
		mat projectionMatrix;
		mat viewMatrix;
		void generateProjectionMatrix();

		//Graphical settings
		bool shadowsEnabled;
	};

}