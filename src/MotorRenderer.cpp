#include "MotorRenderer.h"
#include "MotorLogger.h"
#include <GL/glew.h>

namespace Motor {
	
	Renderer::Renderer(){
	}

	Renderer::~Renderer(){
	}

	int Renderer::initialize(int width, int height){
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
		return;
	}

	void Renderer::setWindowSize(int width, int height)
	{
		windowWidth = width;
		windowHeight = height;
	}

	bool Renderer::renderFrame(){
		glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		return true;
	}

}