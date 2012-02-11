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
			Logger::getSingleton().log(Logger::WARNING, "Renderer already initialized");
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
		if( shaderManager == 0 ) shaderManager = new ShaderManager;
		if( loadShaders() == false ) return false;

		Logger::getSingleton().log(Logger::INFO, "Shaders loaded");

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

		checkErrors();

		initialized = true;
		return 1;
	}
    
    void Renderer::setUpShadowFBO() {

		// Create the texture for the shadow map

		glGenTextures(1, &shadowTexMapHandle);
		glBindTexture(GL_TEXTURE_2D, shadowTexMapHandle);
		//Set parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
		//Make it 1024*1024 with only a depth component (8 bits).
		//By setting the data pointer to 0 it will allocate a buffer and not fill it yet
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

		//Create the framebuffer object

		glGenFramebuffersEXT(1, &shadowFBOHandle);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, shadowFBOHandle);

		//This will make sure that only depth data is drawn to and read from the framebuffer
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		//Attach the texture to the framebuffer object
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, shadowTexMapHandle, 0);

		GLenum FBOstatus;
		FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if(FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
			Logger::getSingleton().log(Logger::WARNING, "Unable to create Framebuffer object for shadowmapping");

		//Unbind the shadow framebuffer object
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
		generateProjectionMatrix();
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

		// Three render steps:
		// 1. Render scene looking from the lights for shadow maps
		// 2. Render scene from camera with shadow maps applied
		// 3. Call render callbacks: the user interface will register a callback in which it can use
		//		functions from Renderer to draw UI elements.


        ////////////////////////////////////////////////////////////////////////////
        // Step 1: We render the scene from the light and save the texture map     /
        ////////////////////////////////////////////////////////////////////////////

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,shadowFBOHandle);	
        glViewport(0,0,1024,1024);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        glCullFace(GL_FRONT);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); 

        Vector3 lightPos = lights->empty() ? Vector3(0,0,0) : lights->front()->position;
       
        //this should actually be a property of the light
        Vector3 lookAt = Vector3(0.0f, -3.2f, 0.0f);
        
        Vector3 relativePosition = lightPos - lookAt;

        mat rotationMat, translateMat, mvpMatrix;
        
        float pitch = (float)(M_PI / 2) - acos(relativePosition.y / relativePosition.length());
        float yaw = atan2(relativePosition.x, relativePosition.z);
        
        rotationMat.setRotationXYZ( pitch, -yaw, 0 );
        translateMat.setTranslation(-lightPos);
        lightViewMatrix = rotationMat * translateMat;
                
        for( ObjectIterator iter = objects->begin(); iter != objects->end(); ++iter ){
            drawObject( *iter, true );
        }

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glViewport(0,0,windowWidth,windowHeight);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glCullFace(GL_BACK);
        
        
        /////////////////////////////////////////////////////////////////////////////
        // Step 2: We render the scene, and read out the values from the shadow map /
        /////////////////////////////////////////////////////////////////////////////
        
        mat lightningProjection = biasMatrix * projectionMatrixShadow * lightViewMatrix;

        // set uniforms for non-animated objects
        shaderManager->setActiveProgram("shadowTextureLightning");
        shaderManager->getActiveProgram()->setUniform3fv("lightPosition", lightPos.ptr());
        shaderManager->getActiveProgram()->setUniformMatrix4fv("lightViewProjMatrix", lightningProjection);
        shaderManager->getActiveProgram()->setUniform1i("tex", 0);
        shaderManager->getActiveProgram()->setUniform1i("shadow", 7);
        
        // set the same uniforms for animated objects 
        // (there is something called uniform buffer objects, which we might want to look into)
        shaderManager->setActiveProgram("shadowTextureLightningMD2");
        shaderManager->getActiveProgram()->setUniform3fv("lightPosition", lightPos.ptr());
        shaderManager->getActiveProgram()->setUniformMatrix4fv("lightViewProjMatrix", lightningProjection);        
        shaderManager->getActiveProgram()->setUniform1i("tex", 0);
        shaderManager->getActiveProgram()->setUniform1i("shadow", 7);

        for( ObjectIterator iter = objects->begin(); iter != objects->end(); ++iter ){
            drawObject( *iter, false );
        }

        shaderManager->setActiveProgram("TextureLightning");

		//Particle effects
		for( EffectIterator iter = effects->begin(); iter != effects->end(); ++iter ){

		}
        
        /////////////////////////////////////////////////////////////////////////////
        // Step 3: Callbacks (project specific, UI, etc.)                           /
        /////////////////////////////////////////////////////////////////////////////

		return true;
	}

	void Renderer::drawObject(SceneObject* obj, bool depthOnly){
		if( obj->visible == false ) return;
		const Model* model = obj->getModel();
		if( model == 0 ) return;
		const Mesh* mesh = model->getMesh();
		if( mesh == 0 ) return;
		const Material* material = model->getMaterial();
        
        bool animation = false;
        int vertexOffset = 0;
        int vertexOffsetNext = 0;
        
		//Note: state might still be valid pointer if a SceneObject is first given an animated model
		//and later a non-animated model. So also check model->isAnimated
		if(obj->getState() && model->isAnimated()) {
            if(depthOnly) {
                shaderManager->setActiveProgram("shadowMapMD2");
            }
            else {
                shaderManager->setActiveProgram("shadowTextureLightningMD2");
            }

            float interpolation = obj->getState()->timetracker * obj->getState()->fps;
            
            shaderManager->getActiveProgram()->setUniform1f("interpolation", interpolation);
            
            animation = true;
            if(obj->getState()->curr_frame > -1) {
                vertexOffset = obj->getState()->curr_frame * (model->verticesPerFrame()) * 12 * 4;
                vertexOffsetNext = obj->getState()->next_frame * (model->verticesPerFrame()) * 12 * 4;
            }
        }
        else {
            if(depthOnly) {
                shaderManager->setActiveProgram("shadowMap");
            }
            else {
                shaderManager->setActiveProgram("shadowTextureLightning");
            }
            animation = false;
        }

		const mat& mMatrix = obj->getFullMoveMatrix();
		mat mvpMatrix;
        
        if(depthOnly) {
            mvpMatrix = projectionMatrixShadow * lightViewMatrix * mMatrix;
        }
        else {
            mvpMatrix = projectionMatrix * viewMatrix * mMatrix;
            shaderManager->getActiveProgram()->setUniformMatrix4fv("mMatrix", mMatrix);
        }
        
        shaderManager->getActiveProgram()->setUniformMatrix4fv("mvpMatrix", mvpMatrix);

		glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);

		shaderManager->getActiveProgram()->vertexAttribPointer(
            "position", mesh->dimension, mesh->vertexBufferDataType, false, mesh->stride, reinterpret_cast<GLvoid*>(vertexOffset));

		if( mesh->hasColor )
            shaderManager->getActiveProgram()->vertexAttribPointer(
				"color", 4, mesh->vertexBufferDataType, false, mesh->stride, reinterpret_cast<GLvoid*>(vertexOffset + 12));

		if( mesh->hasNormal )
			shaderManager->getActiveProgram()->vertexAttribPointer(
				"normal", 3, mesh->vertexBufferDataType, false, mesh->stride, reinterpret_cast<GLvoid*>(vertexOffset + 28));
        
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
			reinterpret_cast<GLvoid*>(vertexOffset + 40));
        
        if(animation){
            shaderManager->getActiveProgram()->vertexAttribPointer("position_next", 3, mesh->vertexBufferDataType, false, mesh->stride, reinterpret_cast<GLvoid*>(vertexOffsetNext));
            shaderManager->getActiveProgram()->vertexAttribPointer("normal_next", 3, mesh->vertexBufferDataType, false, mesh->stride, reinterpret_cast<GLvoid*>(vertexOffsetNext + 28));
        }

		if( mesh->hasIndexBuffer ){
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
			glDrawElements(mesh->primitiveType, mesh->indexCount, mesh->indexBufferDataType, 0);
		} else{
			glDrawArrays(mesh->primitiveType, 0, mesh->vertexCount);
		}
        
		return;
	}

	bool Renderer::loadShaders(){
		bool success = true;
		
		success &= shaderManager->makeShaderProgram("shadowMap", "shaders/shadowmap.vsh", "shaders/shadowmap.fsh");
		shaderManager->bindAttrib("shadowMap", "position", AT_VERTEX);
		success &= shaderManager->linkProgram("shadowMap");

        success &= shaderManager->makeShaderProgram("shadowMapMD2", "shaders/shadowmapmd2.vsh", "shaders/shadowmapmd2.fsh");
		shaderManager->bindAttrib("shadowMapMD2", "position", AT_VERTEX);
        shaderManager->bindAttrib("shadowMapMD2", "position_next", AT_VERTEX_NEXT);
		success &= shaderManager->linkProgram("shadowMapMD2");
        
		success &= shaderManager->makeShaderProgram("shadowTextureLightning", "shaders/shadowtexturelightning.vsh", "shaders/shadowtexturelightning.fsh");
		shaderManager->bindAttrib("shadowTextureLightning", "textureCoordinate", AT_TEXCOORD);
		shaderManager->bindAttrib("shadowTextureLightning", "position", AT_VERTEX);
		shaderManager->bindAttrib("shadowTextureLightning", "color", AT_COLOR);
		shaderManager->bindAttrib("shadowTextureLightning", "normal", AT_NORMAL);
		success &= shaderManager->linkProgram("shadowTextureLightning");
        
        success &= shaderManager->makeShaderProgram("shadowTextureLightningMD2", "shaders/shadowtexturelightningmd2.vsh", "shaders/shadowtexturelightningmd2.fsh");
		shaderManager->bindAttrib("shadowTextureLightningMD2", "textureCoordinate", AT_TEXCOORD);
		shaderManager->bindAttrib("shadowTextureLightningMD2", "position", AT_VERTEX);
		shaderManager->bindAttrib("shadowTextureLightningMD2", "color", AT_COLOR);
		shaderManager->bindAttrib("shadowTextureLightningMD2", "normal", AT_NORMAL);
        shaderManager->bindAttrib("shadowTextureLightningMD2", "normal_next", AT_NORMAL_NEXT);
        shaderManager->bindAttrib("shadowTextureLightningMD2", "position_next", AT_VERTEX_NEXT);
		success &= shaderManager->linkProgram("shadowTextureLightningMD2");

		success &= shaderManager->makeShaderProgram("TextureLightning", "shaders/texturelightning.vsh", "shaders/texturelightning.fsh");
		shaderManager->bindAttrib("TextureLightning", "textureCoordinate", AT_TEXCOORD);
		shaderManager->bindAttrib("TextureLightning", "position", AT_VERTEX);
		shaderManager->bindAttrib("TextureLightning", "color", AT_COLOR);
		shaderManager->bindAttrib("TextureLightning", "normal", AT_NORMAL);
		success &= shaderManager->linkProgram("TextureLightning");

		success &= shaderManager->makeShaderProgram("Ortho", "shaders/orthogonal.vsh", "shaders/orthogonal.fsh");
		shaderManager->bindAttrib("Ortho", "textureCoordinate", AT_TEXCOORD);
		shaderManager->bindAttrib("Ortho", "position", AT_VERTEX);
		success &= shaderManager->linkProgram("Ortho");
		
		if( success == false ){
			shaderManager->unloadAllShaders();
		}
		//Display any OpenGL error code that might have been generated
		checkErrors();
		
		return success;
	}

	void Renderer::generateProjectionMatrix(){
		float invAspect = (float)windowHeight / (float)windowWidth;
		float near = 2.0f, far = 200.0f, fov = 60.0f * (float)M_PI/180.0f;
		float xmax = near * (float)tan(0.5f*fov);
		float xmin = -xmax;
		projectionMatrix.setPerspective(xmin, xmax, xmin*invAspect, xmax*invAspect, near, far);
        
		fov = 100.0f * (float)M_PI/180.0f;
		xmax = near * (float)tan(0.5f*fov);
		xmin = -xmax;
		far = 50.0f;
        projectionMatrixShadow.setPerspective(xmin, xmax, xmin, xmax, near, far);
    }
}
