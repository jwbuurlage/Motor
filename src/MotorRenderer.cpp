#include "MotorRenderer.h"
#include "MotorShaderManager.h"
#include "MotorLogger.h"
#include "MotorSceneObject.h"
#include "MotorMesh.h"
#include "MotorMaterial.h"
#include "MotorTexture.h"
#include <GL/glew.h>
#include <sstream>

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
		windowWidth = width;
		windowHeight = height;

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

	void Renderer::setWindowSize(int width, int height){
		windowWidth = width;
		windowHeight = height;
	}

	bool Renderer::checkErrors(){
		GLenum err = glGetError();
		if( err ){
			std::stringstream errorText;
			errorText << "OpenGL error occured. Code: " << err;
			Logger::getSingleton().log(Logger::ERROR, errorText.str().c_str() );
			return false;
		}
		return true;
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
		Vector3 lightPos(2.0f, 3.0f, 4.0f);
		shaderManager->setActiveProgram("TextureLightning");
		shaderManager->getActiveProgram()->setUniform1i("tex", 0);
		shaderManager->getActiveProgram()->setUniform3fv("lightPosition", lightPos);
		for( ObjectIterator iter = objects->begin(); iter != objects->end(); ++iter ){
			drawObject( *iter );
		}

		//
		//Step 3
		//

		return true;
	}

	void Renderer::drawObject(SceneObject* obj){
		const Mesh* mesh = obj->getMesh();
		if( mesh == 0 ) return;

		mat mMatrix, mvpMatrix;

		//Rotate: first roll, then pitch, then yaw
		mMatrix.setRotationZ(obj->roll);
		mMatrix.rotateX(obj->pitch);
		mMatrix.rotateY(obj->yaw);
		mMatrix.scale(obj->scale);
		mMatrix.translate(obj->position);

		mvpMatrix = projectionMatrix * viewMatrix * mMatrix;

		shaderManager->getActiveProgram()->setUniformMatrix4fv("mMatrix", mMatrix);
		shaderManager->getActiveProgram()->setUniformMatrix4fv("mvpMatrix", mvpMatrix);

		glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);

		shaderManager->getActiveProgram()->vertexAttribPointer(
			"position",
			mesh->dimension,
			mesh->vertexBufferDataType,
			false,
			mesh->stride,
			mesh->vertexOffset);

		if( mesh->hasColor )
			shaderManager->getActiveProgram()->vertexAttribPointer(
				"color",
				4,
				mesh->vertexBufferDataType,
				false,
				mesh->stride,
				(GLvoid*)12);

		if( mesh->hasNormal )
			shaderManager->getActiveProgram()->vertexAttribPointer(
				"normal",
				3,
				mesh->vertexBufferDataType,
				false,
				mesh->stride,
				(GLvoid*)28);

		if( mesh->material ){
			glEnable(GL_TEXTURE_2D);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh->material->texture->handle);
			shaderManager->getActiveProgram()->vertexAttribPointer(
				"textureCoordinate",
				2,
				mesh->vertexBufferDataType,
				false,
				mesh->stride,
				(GLvoid*)40);
		}else{
			glDisable(GL_TEXTURE_2D);
		}

		if( mesh->hasIndexBuffer ){
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
			glDrawElements(mesh->primitiveType, mesh->indexCount, mesh->indexBufferDataType, 0);
		}else{
			glDrawArrays(mesh->primitiveType, 0, mesh->vertexCount);
		}

		return;
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
		float near = 2.0f, far = 80.0f, fov = 60.0f * (float)M_PI/360.0f;
		float xmax = near * (float)tan(fov);
		float xmin = -xmax;
		projectionMatrix.setPerspective(xmin, xmax, xmin*invAspect, xmax*invAspect, near, far);
	}
}