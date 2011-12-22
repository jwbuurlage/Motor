#include "MotorRenderer.h"
#include "MotorShaderManager.h"
#include "MotorLogger.h"
#include <GL/glew.h>

#define _USE_MATH_DEFINES
#include <math.h>

//Vertex attribute enums
//See: http://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/attributes.php
//enums for the uniforms and attributes
enum {
    AT_VERTEX,
    AT_VERTEX_NEXT,
    AT_COLOR,
    AT_NORMAL,
    AT_NORMAL_NEXT,
    AT_TEXCOORD,
    ATTRIBUTE_COUNT
};

namespace Motor {
	
	Renderer::Renderer(){
		initialized = false;
		shaderManager = 0;
	}

	Renderer::~Renderer(){
		cleanup();
	}

	int Renderer::initialize(int width, int height){
		if( initialized ){
			Logger::getSingleton().log(Logger::WARNING, "Renderer already initializedd");
			return 1;
		}
		if( glewInit() != GLEW_OK ){
			Logger::getSingleton().log(Logger::CRITICALERROR, "glewInit failed");
			return 0;
		}
		if( !GLEW_VERSION_2_0 ){
			Logger::getSingleton().log(Logger::CRITICALERROR, "OpenGL 2.0 not supported");
			return 0;
		}
		shaderManager = new ShaderManager;
		loadShaders();

		//Set OpenGL settings
		glShadeModel(GL_SMOOTH);	
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	
		glClearDepth(1.0);
		glEnable(GL_CULL_FACE);

		glViewport(0, 0, (GLsizei)windowWidth, (GLsizei)windowHeight);
		generateProjectionMatrix();

		//Get graphical settings from setting class (TODO: SettingManager)
		shadowsEnabled = true;

		initialized = true;
		return 1;
	}

	void Renderer::cleanup(){
		if( shaderManager ) delete shaderManager;
		shaderManager = 0;
		return;
	}

	void Renderer::setWindowSize(int width, int height)
	{
		windowWidth = width;
		windowHeight = height;
	}

	bool Renderer::renderFrame(){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//
		// Three render steps:
		// 1. Render scene looking from the light for shadow maps
		// 2. Render scene from camera with shadow maps applied
		// 3. Call render callbacks: the user interface will register a callback in which it can use
		//		functions from Renderer to draw UI elements.
		//

		//
		//Step 1
		//
		if( shadowsEnabled ){


		}

		//
		//Step 2
		//
		

		//
		//Step 3
		//

		return true;
	}

	void Renderer::loadShaders(){
		shaderManager->makeShaderProgram("shadowMap", "shaders/shadowmap.vsh", "shaders/shadowmap.fsh");
		shaderManager->bindAttrib("shadowMap", "position", AT_VERTEX);
		shaderManager->linkProgram("shadowMap");

		shaderManager->makeShaderProgram("shadowTextureLightning", "shaders/shadowtexturelightning.vsh", "shaders/shadowtexturelightning.fsh");
		shaderManager->bindAttrib("shadowTextureLightning", "textureCoordinate", AT_TEXCOORD);
		shaderManager->bindAttrib("shadowTextureLightning", "position", AT_VERTEX);
		shaderManager->bindAttrib("shadowTextureLightning", "color", AT_COLOR);
		shaderManager->bindAttrib("shadowTextureLightning", "normal", AT_NORMAL);
		shaderManager->linkProgram("shadowTextureLightning");

		shaderManager->makeShaderProgram("TextureLightning", "shaders/texturelightning.vsh", "shaders/texturelightning.fsh");
		shaderManager->bindAttrib("TextureLightning", "textureCoordinate", AT_TEXCOORD);
		shaderManager->bindAttrib("TextureLightning", "position", AT_VERTEX);
		shaderManager->bindAttrib("TextureLightning", "color", AT_COLOR);
		shaderManager->bindAttrib("TextureLightning", "normal", AT_NORMAL);
		shaderManager->linkProgram("TextureLightning");

		shaderManager->makeShaderProgram("Ortho", "shaders/orthogonal.vsh", "shaders/orthogonal.fsh");
		shaderManager->bindAttrib("Ortho", "textureCoordinate", AT_TEXCOORD);
		shaderManager->bindAttrib("Ortho", "position", AT_VERTEX);
		shaderManager->linkProgram("Ortho");
	}

	void Renderer::generateProjectionMatrix(){
		float invAspect = (float)windowHeight / (float)windowWidth;
		float near = 2.0f, far = 80.0f, fov = 60.0f * M_PI/360.0f;
		float xmax = near * (float)tan(fov);
		float xmin = -xmax;
		projectionMatrix.setPerspective(xmin, xmax, xmin*invAspect, xmax*invAspect, near, far);
	}
}