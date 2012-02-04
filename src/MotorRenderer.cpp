#include "MotorRenderer.h"
#include "MotorShaderManager.h"
#include "MotorLogger.h"
#include "MotorSceneObject.h"
#include "MotorLight.h"
#include "MotorModel.h"
#include "MotorMesh.h"
#include "MotorMaterial.h"
#include "MotorTexture.h"
#include "MotorTextureManager.h"
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
        setUpShadowFBO();
        
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D,shadowTexMapHandle);
        glActiveTexture(GL_TEXTURE0);
        
        biasMatrix.setIdentity();
        biasMatrix.scale(0.5f);
        biasMatrix.translate(0.5f, 0.5f, 0.5f);


		initialized = true;
		return 1;
	}
    
    void Renderer::setUpShadowFBO() {
            //debug variable
            GLenum FBOstatus;
            
            //try to use a texture depth component
            glGenTextures(1, &shadowTexMapHandle);
            glBindTexture(GL_TEXTURE_2D, shadowTexMapHandle);
            
            //we set some parameters to minimize unwanted artifacts
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
            
            //create and bind a texture that will hold the shadowmap
            glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,  1024, 1024, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            
            //create a framebuffer object
            glGenFramebuffersEXT(1, &shadowFBOHandle);
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, shadowFBOHandle);
            
            //we need to instruct opengl that we're just interested in depth data
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            
            //we now attach the texture to the depth component of the FBO
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, shadowTexMapHandle, 0);
            
            //we need to debug the link
            FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
            if(FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
                printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");
            
            //finally we switch back to the window-system buffer
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

        return;
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
		// 1. Render scene looking from the lights for shadow maps
		// 2. Render scene from camera with shadow maps applied
		// 3. Call render callbacks: the user interface will register a callback in which it can use
		//		functions from Renderer to draw UI elements.
		//

		if( shadowsEnabled ){
            //render to shadowmap
            
            //
            //Step 1
            //


            ////////////////////////////////////////////////////////////////////////////
            // First pass: We render the scene from the light and save the texture map /
            ////////////////////////////////////////////////////////////////////////////

            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,shadowFBOHandle);	
            glViewport(0,0,1024,1024);
            glClear( GL_DEPTH_BUFFER_BIT);
            
            glCullFace(GL_FRONT);
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); 

            Vector3 lightPos = lights->empty() ? Vector3(0,0,0) : lights->front()->position;
            Vector3 lookAt = Vector3(0.0f, -3.2f, 0.0f);
            Vector3 relativePosition = lightPos - lookAt;

            mat rotationMat, translateMat, mvpMatrix;
            
            float pitch = (float)(M_PI / 2) - acos(relativePosition.y / relativePosition.length());
            float yaw = atan2(relativePosition.x, relativePosition.z);
            
            rotationMat.setRotationXYZ( pitch, -yaw, 0 );
            translateMat.setTranslation(-lightPos);
            lightViewMatrix = rotationMat * translateMat;
            
            shaderManager->setActiveProgram("shadowMap");
            
            for( ObjectIterator iter = objects->begin(); iter != objects->end(); ++iter ){
                drawObjectDepthOnly( *iter );
            }
            
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
            glViewport(0,0,windowWidth,windowHeight);
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            glCullFace(GL_BACK);
            
            //
            //Step 2
            //

            shaderManager->setActiveProgram("shadowTextureLightning");
            shaderManager->getActiveProgram()->setUniform3fv("lightPosition", lightPos.ptr());
            mat lightningProjection = biasMatrix * projectionMatrix * lightViewMatrix;
            shaderManager->getActiveProgram()->setUniformMatrix4fv("lightMatrix", lightningProjection);
            
            shaderManager->getActiveProgram()->setUniform1i("tex", 0);
            shaderManager->getActiveProgram()->setUniform1i("shadow", 7);
            
            for( ObjectIterator iter = objects->begin(); iter != objects->end(); ++iter ){
                drawObject( *iter );
            }



		}
        else {
            shaderManager->setActiveProgram("TextureLightning");
        

            //
            //Step 2
            //
            shaderManager->getActiveProgram()->setUniform1i("tex", 0);
            //TODO: get the 8 closests lights!
            int lightCounter = 0;
            for( LightIterator iter = lights->begin(); iter != lights->end(); ++iter ){
                if( (*iter)->enabled ){
                    std::stringstream varname;
                    varname << "lightPosition[" << lightCounter << "]";
                    shaderManager->getActiveProgram()->setUniform3fv(varname.str().c_str(), (*iter)->position );
                    ++lightCounter;
                }
            }
            shaderManager->getActiveProgram()->setUniform1i("lightCount", lightCounter);

            for( ObjectIterator iter = objects->begin(); iter != objects->end(); ++iter ){
                drawObject( *iter );
            }
                
        }
        
        shaderManager->setActiveProgram("TextureLightning");

		//Particle effects
		for( EffectIterator iter = effects->begin(); iter != effects->end(); ++iter ){

		}

		//
		//Step 3
		//

		return true;
	}
    
    void Renderer::drawObjectDepthOnly(SceneObject* obj) {
        
        const Model* model = obj->getModel();
		if( model == 0 ) return;
		const Mesh* mesh = model->getMesh();
		if( mesh == 0 ) return;
		const Material* material = model->getMaterial();
        
		mat mMatrix, mvpMatrix;
        
		//Rotate: first roll, then pitch, then yaw
		mMatrix.setRotationZ(obj->roll);
		mMatrix.rotateX(obj->pitch);
		mMatrix.rotateY(obj->yaw);
		mMatrix.scale(obj->scale);
		mMatrix.translate(obj->position);
        
		mvpMatrix = projectionMatrix * viewMatrix * mMatrix;
                
        mvpMatrix = projectionMatrix * lightViewMatrix * mMatrix;
        
        //set the uniform
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
        
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
        
		if( material && material->texture ){
			glBindTexture(GL_TEXTURE_2D, model->getMaterial()->texture->handle);
		}else{
			glBindTexture(GL_TEXTURE_2D, TextureManager::getSingleton().getTexture("default")->handle);
		}
		shaderManager->getActiveProgram()->vertexAttribPointer(
                                                               "textureCoordinate",
                                                               2,
                                                               mesh->vertexBufferDataType,
                                                               false,
                                                               mesh->stride,
                                                               (GLvoid*)40);
        
		if( mesh->hasIndexBuffer ){
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
			glDrawElements(mesh->primitiveType, mesh->indexCount, mesh->indexBufferDataType, 0);
		}else{
			glDrawArrays(mesh->primitiveType, 0, mesh->vertexCount);
		}
        
		return;

    }

	void Renderer::drawObject(SceneObject* obj){
		const Model* model = obj->getModel();
		if( model == 0 ) return;
		const Mesh* mesh = model->getMesh();
		if( mesh == 0 ) return;
		const Material* material = model->getMaterial();

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

		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);

		if( material && material->texture ){
			glBindTexture(GL_TEXTURE_2D, model->getMaterial()->texture->handle);
		}else{
			glBindTexture(GL_TEXTURE_2D, TextureManager::getSingleton().getTexture("default")->handle);
		}
		shaderManager->getActiveProgram()->vertexAttribPointer(
			"textureCoordinate",
			2,
			mesh->vertexBufferDataType,
			false,
			mesh->stride,
			(GLvoid*)40);

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