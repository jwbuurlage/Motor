#include "MotorRenderer.h"
#include "MotorShaderManager.h"
#include "MotorLogger.h"
#include <GL/glew.h>

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
		glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		return true;
	}

	void Renderer::loadShaders(){
		shaderManager->makeShader("shadowMapVertex", ShaderManager::Vertex, "shaders/shadowMap.vsh");
		shaderManager->makeShader("shadowMapFragment", ShaderManager::Fragment, "shaders/shadowMap.fsh");
		shaderManager->makeShaderProgram("shadowMap", "shadowMapVertex", "shadowMapFragment");
		shaderManager->bindAttrib("shadowMap", "position", AT_VERTEX);
		shaderManager->linkProgram("shadowMap");

		shaderManager->makeShader("stlVert", ShaderManager::Vertex, "shaders/shadowTextureLightning.vsh");
		shaderManager->makeShader("stlFrag", ShaderManager::Fragment, "shaders/shadowTextureLightning.fsh");
		shaderManager->makeShaderProgram("shadowTextureLightning", "stlVert", "stlFrag");
		shaderManager->bindAttrib("shadowTextureLightning", "textureCoordinate", AT_TEXCOORD);
		shaderManager->bindAttrib("shadowTextureLightning", "position", AT_VERTEX);
		shaderManager->bindAttrib("shadowTextureLightning", "color", AT_COLOR);
		shaderManager->bindAttrib("shadowTextureLightning", "normal", AT_NORMAL);
		shaderManager->linkProgram("shadowTextureLightning");

		shaderManager->makeShader("TLVertex", ShaderManager::Vertex, "shaders/texturelightning.vsh");
		shaderManager->makeShader("TLFragment", ShaderManager::Fragment, "shaders/texturelightning.fsh");
		shaderManager->makeShaderProgram("TextureLightning", "TLVertex", "TLFragment");
		shaderManager->bindAttrib("TextureLightning", "textureCoordinate", AT_TEXCOORD);
		shaderManager->bindAttrib("TextureLightning", "position", AT_VERTEX);
		shaderManager->bindAttrib("TextureLightning", "color", AT_COLOR);
		shaderManager->bindAttrib("TextureLightning", "normal", AT_NORMAL);
		shaderManager->linkProgram("TextureLightning");

		shaderManager->makeShader("OrthoVertex", ShaderManager::Vertex, "shaders/orthogonal.vsh");
		shaderManager->makeShader("OrthoFragment", ShaderManager::Fragment, "shaders/orthogonal.fsh");
		shaderManager->makeShaderProgram("Ortho", "OrthoVertex", "OrthoFragment");
		shaderManager->bindAttrib("Ortho", "textureCoordinate", AT_TEXCOORD);
		shaderManager->bindAttrib("Ortho", "position", AT_VERTEX);
		shaderManager->linkProgram("Ortho");
	}

}