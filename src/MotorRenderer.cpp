#include "MotorRenderer.h"
#include "MotorShaderManager.h"
#include "MotorLogger.h"
#include "MotorSceneObject.h"
#include "MotorLight.h"
#include "MotorParticleEffect.h"
#include "MotorModel.h"
#include "MotorMesh.h"
#include "MotorMaterial.h"
#include "MotorTexture.h"
#include "MotorTextureManager.h"
#include "MotorTerrain.h"
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
	AT_ALPHA
};

namespace Motor {
	
	Renderer::Renderer(){
		initialized = false;
		shaderManager = 0;
		particleBuffer = 0;
		terrain = 0;
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
		if( loadShaders() == false ){
			Logger::getSingleton().log(Logger::ERROR, "The shaders failed to load. Motor not usable.");
			delete shaderManager;
			shaderManager = 0;
			return false;
		}

		Logger::getSingleton().log(Logger::INFO, "Shaders loaded");

		//Set OpenGL settings
		glShadeModel(GL_SMOOTH);	
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	
		glClearDepth(1.0);
		glEnable(GL_CULL_FACE);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

		particleBufferSize = 0;

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
		if( particleBuffer ) delete[] particleBuffer;
		particleBuffer = 0;
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
			Logger::getSingleton() << Logger::ERROR << "OpenGL error occurred. Code: " << err << endLog;
			return false;
		}
		return true;
	}

	bool Renderer::renderFrame(){
		// Three render steps:
		// 1. Render scene looking from the lights for shadow maps
		// 2. Render scene from camera with shadow maps applied
		// 3. Call render callbacks: the user interface will register a callback in which it can use
		//		functions from Renderer to draw UI elements.

        Vector3 lightPos = lights->empty() ? Vector3(0,0,0) : lights->front()->position;
        //this should actually be a property of the light
        Vector3 lookAt = Vector3(0.0f, -3.2f, 0.0f);

        ////////////////////////////////////////////////////////////////////////////
        // Step 1: We render the scene from the light and save the texture map     /
        ////////////////////////////////////////////////////////////////////////////

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,shadowFBOHandle);	
        glViewport(0,0,1024,1024);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        glCullFace(GL_FRONT);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); 
        
        Vector3 relativePosition = lightPos - lookAt;

        mat lightViewMat; //Initialized to identity
        
        float pitch = (float)(M_PI / 2) - acos(relativePosition.y / relativePosition.length());
        float yaw = atan2(relativePosition.x, relativePosition.z);
        
		lightViewMat.translate(-lightPos);
		lightViewMat.rotateXYZ( pitch, -yaw, 0 );

		projViewMatrix = projectionMatrixShadow;
		projViewMatrix *= lightViewMat;

		//To reduce the amount of shader switches we sort by shaders: first the MD2 models, then the rest
        for( ObjectIterator iter = objects->begin(); iter != objects->end(); ++iter ){
			if( (*iter)->getModel() && (*iter)->getModel()->isAnimated() ){
				drawObject( *iter, true );
			}
        }
        for( ObjectIterator iter = objects->begin(); iter != objects->end(); ++iter ){
			if( (*iter)->getModel() && !(*iter)->getModel()->isAnimated() ){
				drawObject( *iter, true );
			}
        }

        /////////////////////////////////////////////////////////////////////////////
        // Step 2: We render the scene, and read out the values from the shadow map /
        /////////////////////////////////////////////////////////////////////////////

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glViewport(0,0,windowWidth,windowHeight);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glCullFace(GL_BACK);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
        
        
        mat lightningProjection = biasMatrix * projViewMatrix;

        // set uniforms for non-animated objects
        shaderManager->setActiveProgram("shadowTextureLightning");
        shaderManager->getActiveProgram()->setUniform3fv("lightPosition", lightPos.ptr());
        shaderManager->getActiveProgram()->setUniformMatrix4fv("lightViewProjMatrix", lightningProjection);
        
        // set the same uniforms for animated objects 
        // (there is something called uniform buffer objects, which we might want to look into)
        shaderManager->setActiveProgram("shadowTextureLightningMD2");
        shaderManager->getActiveProgram()->setUniform3fv("lightPosition", lightPos.ptr());
        shaderManager->getActiveProgram()->setUniformMatrix4fv("lightViewProjMatrix", lightningProjection);        

		projViewMatrix = projectionMatrix;
		projViewMatrix *= viewMatrix;

        for( ObjectIterator iter = objects->begin(); iter != objects->end(); ++iter ){
			if( (*iter)->getModel() && (*iter)->getModel()->isAnimated() ){
				drawObject( *iter, false );
			}
        }
        for( ObjectIterator iter = objects->begin(); iter != objects->end(); ++iter ){
			if( (*iter)->getModel() && !(*iter)->getModel()->isAnimated() ){
				drawObject( *iter, false );
			}
		}

		//
		// Terrain test
		//
		shaderManager->setActiveProgram("terrain");
        shaderManager->getActiveProgram()->setUniform3fv("lightPosition", lightPos.ptr());
        shaderManager->getActiveProgram()->setUniformMatrix4fv("lightViewProjMatrix", lightningProjection);  
		shaderManager->getActiveProgram()->setUniform1i("shadowMap", 7);
		drawTerrain();
		
		//Particle effects
		
		//Make a buffer to hold all vertex data
		int minBufferSize = 0;
		for( EffectIterator iter = effects->begin(); iter != effects->end(); ++iter ){
			int sizeNeeded = (*iter)->particles.size()*24; //24 floats per particle
			if( sizeNeeded > minBufferSize ) minBufferSize = sizeNeeded;
		}
		if( particleBufferSize < minBufferSize ){
			if( particleBuffer ) delete[] particleBuffer;
			particleBuffer = new GLfloat[minBufferSize];
			particleBufferSize = minBufferSize;
		}

		shaderManager->setActiveProgram("particlefx");
		shaderManager->getActiveProgram()->setUniformMatrix4fv("pMatrix", projectionMatrix);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		for( EffectIterator iter = effects->begin(); iter != effects->end(); ++iter ){
			drawParticleEffect(*iter);
		}
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
        
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
        
        shaderManager->getActiveProgram()->setUniformMatrix4fv("mvpMatrix", projViewMatrix * mMatrix);
        if(depthOnly == false)
            shaderManager->getActiveProgram()->setUniformMatrix4fv("mMatrix", mMatrix);

		glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);

		shaderManager->getActiveProgram()->vertexAttribPointer(
            AT_VERTEX, mesh->dimension, mesh->vertexBufferDataType, false, mesh->stride, reinterpret_cast<GLvoid*>(vertexOffset));

		if( depthOnly == false ){
			if( mesh->hasColor )
				shaderManager->getActiveProgram()->vertexAttribPointer(
					AT_COLOR, 4, mesh->vertexBufferDataType, false, mesh->stride, reinterpret_cast<GLvoid*>(vertexOffset + 12));

			if( mesh->hasNormal )
				shaderManager->getActiveProgram()->vertexAttribPointer(
					AT_NORMAL, 3, mesh->vertexBufferDataType, false, mesh->stride, reinterpret_cast<GLvoid*>(vertexOffset + 28));

			if( material && material->texture ){
				glBindTexture(GL_TEXTURE_2D, material->texture->handle);
			}else{
				glBindTexture(GL_TEXTURE_2D, TextureManager::getSingleton().getTexture("default")->handle);
			}

			shaderManager->getActiveProgram()->vertexAttribPointer(
				AT_TEXCOORD,
				2,
				mesh->vertexBufferDataType,
				false,
				mesh->stride,
				reinterpret_cast<GLvoid*>(vertexOffset + 40));
		}

        if(animation){
			shaderManager->getActiveProgram()->vertexAttribPointer(AT_VERTEX_NEXT, 3, mesh->vertexBufferDataType, false, mesh->stride, reinterpret_cast<GLvoid*>(vertexOffsetNext));
			shaderManager->getActiveProgram()->vertexAttribPointer(AT_NORMAL_NEXT, 3, mesh->vertexBufferDataType, false, mesh->stride, reinterpret_cast<GLvoid*>(vertexOffsetNext + 28));
        }

		if( mesh->hasIndexBuffer ){
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
			glDrawElements(mesh->primitiveType, mesh->indexCount, mesh->indexBufferDataType, 0);
		} else{
			glDrawArrays(mesh->primitiveType, 0, mesh->vertexCount);
		}
        
		return;
	}
	
	void Renderer::drawParticleEffect(ParticleEffect* fx){
		if( fx == 0 ) return;
		if( fx->particles.empty() ) return;

		if( fx->material && fx->material->texture )
			glBindTexture(GL_TEXTURE_2D, fx->material->texture->handle);
		else
			glBindTexture(GL_TEXTURE_2D, TextureManager::getSingleton().getTexture("default")->handle);


		bool fadeIn = (fx->fadeInEnd > 0.001f);
		bool fadeOut = (fx->fadeOutStart+0.001f < fx->lifeTime);
		float fadeInCompare = fx->lifeTime - fx->fadeInEnd;
		float fadeOutCompare = fx->lifeTime - fx->fadeOutStart;

		//Generate a buffer with all the quads
		GLfloat* bufPointer = particleBuffer;
		for( std::vector<Particle>::iterator part = fx->particles.begin(); part != fx->particles.end(); ++part ){
			//Get eye space coordinates (billboarding)
			Vector3 pos(viewMatrix * part->position);
			pos.x -= part->size/2;
			pos.y -= part->size;
			float alpha;
			if( fadeIn && (part->timeLeft > fadeInCompare) ){
				alpha = (fx->lifeTime - part->timeLeft) / fx->fadeInEnd;
			}else if( fadeOut && (part->timeLeft < fadeOutCompare) ){
				alpha = part->timeLeft / (fx->lifeTime - fx->fadeOutStart);
			}else{
				alpha = 1.0f;
			}

			//left-bottom
			*bufPointer++ = 0.0f; //Texture s
			*bufPointer++ = 1.0f; //Texture t
			*bufPointer++ = alpha;
			*bufPointer++ = pos.x;
			*bufPointer++ = pos.y;
			*bufPointer++ = pos.z;

			*bufPointer++ = 1.0f; //Texture s
			*bufPointer++ = 1.0f; //Texture t
			*bufPointer++ = alpha;
			*bufPointer++ = pos.x + part->size;
			*bufPointer++ = pos.y;
			*bufPointer++ = pos.z;

			*bufPointer++ = 1.0f; //Texture s
			*bufPointer++ = 0.0f; //Texture t
			*bufPointer++ = alpha;
			*bufPointer++ = pos.x + part->size;
			*bufPointer++ = pos.y + part->size;
			*bufPointer++ = pos.z;

			*bufPointer++ = 0.0f; //Texture s
			*bufPointer++ = 0.0f; //Texture t
			*bufPointer++ = alpha;
			*bufPointer++ = pos.x;
			*bufPointer++ = pos.y + part->size;
			*bufPointer++ = pos.z;
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0); //Buffer is just a memory pointer, not in video memory buffer
		glEnableVertexAttribArray(AT_TEXCOORD);
		glEnableVertexAttribArray(AT_VERTEX);
		glEnableVertexAttribArray(AT_ALPHA);
		glVertexAttribPointer(AT_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 24, particleBuffer);
		glVertexAttribPointer(AT_ALPHA, 1, GL_FLOAT, GL_FALSE, 24, particleBuffer+2);
		glVertexAttribPointer(AT_VERTEX, 3, GL_FLOAT, GL_FALSE, 24, particleBuffer+3);

		glDrawArrays(GL_QUADS, 0, fx->particles.size()*4);
	}

	void Renderer::drawTerrain(){
        glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, terrain->heightMap->handle);
		shaderManager->getActiveProgram()->setUniform1i("heightMap", 0); //GL_TEXTURE0
      
        glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, terrain->normalMap->handle);
		shaderManager->getActiveProgram()->setUniform1i("normalMap", 1); //GL_TEXTURE1
		glActiveTexture(GL_TEXTURE0); //because the rest of renderer expects texture 0 to be set

		shaderManager->getActiveProgram()->setUniformMatrix4fv("mMatrix", terrain->scaleMatrix);
		shaderManager->getActiveProgram()->setUniformMatrix4fv("vpMatrix", projViewMatrix);

		glBindBuffer(GL_ARRAY_BUFFER, terrain->vertexBuffer);
		shaderManager->getActiveProgram()->vertexAttribPointer(AT_TEXCOORD, 2, GL_FLOAT, false, 0, 0);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_CULL_FACE);
        for(int i = 0; i < terrain->patchCount() * terrain->patchCount(); ++i) {     
            if(terrain->patches[i].lod < 1) {
                //red
                shaderManager->getActiveProgram()->setUniform3fv("colorUniform", Vector3(1.0f, 0.0f, 0.0f).ptr());
            }
            else if(terrain->patches[i].lod < 2) {
                //green
                shaderManager->getActiveProgram()->setUniform3fv("colorUniform", Vector3(0.0f, 1.0f, 0.0f).ptr());
            }
            else if(terrain->patches[i].lod < 3) {
                //blue
                shaderManager->getActiveProgram()->setUniform3fv("colorUniform", Vector3(0.0f, 0.0f, 1.0f).ptr());
            }
            else {
                //gray
                shaderManager->getActiveProgram()->setUniform3fv("colorUniform", Vector3(0.5f, 0.5f, 0.5f).ptr());
            }
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain->indexBuffer[terrain->patches[i].lod]);
            glUniform2fv(shaderManager->getActiveProgram()->getUniformLocation("delta"), 1, terrain->patches[i].offset);
            glDrawElements(GL_TRIANGLE_STRIP, terrain->indexCount[terrain->patches[i].lod], GL_UNSIGNED_INT, 0);            
        }
        
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_CULL_FACE);

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain->indexBuffer);
		//glDrawElements(GL_TRIANGLE_STRIP, terrain->indexCount, GL_UNSIGNED_SHORT, 0);

		return;
	}


	bool Renderer::loadShaders(){
		bool success = true;
		
		ShaderManager::Shader* shadowFS = shaderManager->makeShader("ShadowFS", ShaderManager::Fragment, "shaders/shadowmap.fsh");
		ShaderManager::Shader* shadowVS = shaderManager->makeShader("ShadowVS", ShaderManager::Vertex, "shaders/shadowmap.vsh");
		ShaderManager::Shader* shadowVSMD2 = shaderManager->makeShader("ShadowVSMD2", ShaderManager::Vertex, "shaders/shadowmapmd2.vsh");
		
		if( shadowFS && shadowVS && shadowVSMD2 ){

			ShaderManager::ShaderProgram* shadowMap = shaderManager->createProgram("shadowMap");
			shadowMap->attachShader(shadowFS);
			shadowMap->attachShader(shadowVS);
			shadowMap->bindAttrib(AT_VERTEX, "position");
			if( !shadowMap->link() ) success = false;

			ShaderManager::ShaderProgram* shadowMapMD2 = shaderManager->createProgram("shadowMapMD2");
			shadowMapMD2->attachShader(shadowFS);
			shadowMapMD2->attachShader(shadowVSMD2);
			shadowMapMD2->bindAttrib(AT_VERTEX, "position");
			shadowMapMD2->bindAttrib(AT_VERTEX_NEXT, "position_next");
			if( !shadowMapMD2->link() ) success = false;

		}else success = false;
        
		if( shaderManager->makeShaderProgram("shadowTextureLightning", "shaders/shadowtexturelightning.vsh", "shaders/shadowtexturelightning.fsh") ){
			shaderManager->bindAttrib("shadowTextureLightning", "textureCoordinate", AT_TEXCOORD);
			shaderManager->bindAttrib("shadowTextureLightning", "position", AT_VERTEX);
			shaderManager->bindAttrib("shadowTextureLightning", "color", AT_COLOR);
			shaderManager->bindAttrib("shadowTextureLightning", "normal", AT_NORMAL);
			if( shaderManager->linkProgram("shadowTextureLightning") ){
				shaderManager->setActiveProgram("shadowTextureLightning");
				shaderManager->getActiveProgram()->setUniform1i("tex", 0);
				shaderManager->getActiveProgram()->setUniform1i("shadow", 7);
			}else success = false;
		}else success = false;
        
		ShaderManager::ShaderProgram* MD2Shader = shaderManager->makeShaderProgram("shadowTextureLightningMD2", "shaders/shadowtexturelightningmd2.vsh", "shaders/shadowtexturelightningmd2.fsh");
        if( MD2Shader ){
			MD2Shader->bindAttrib(AT_TEXCOORD, "textureCoordinate");
			MD2Shader->bindAttrib(AT_VERTEX, "position");
			MD2Shader->bindAttrib(AT_COLOR, "color");
			MD2Shader->bindAttrib(AT_NORMAL, "normal");
			MD2Shader->bindAttrib(AT_NORMAL_NEXT, "normal_next");
			MD2Shader->bindAttrib(AT_VERTEX_NEXT, "position_next");
			if( MD2Shader->link() ){
				MD2Shader->use();
				MD2Shader->setUniform1i("tex", 0);
				MD2Shader->setUniform1i("shadow", 7);
			}else success = false;
		}else success = false;

		ShaderManager::ShaderProgram* fxShader = shaderManager->makeShaderProgram("particlefx", "shaders/particlefx.vsh", "shaders/particlefx.fsh");
		if( fxShader ){
			fxShader->bindAttrib(AT_VERTEX, "position");
			fxShader->bindAttrib(AT_ALPHA, "alpha");
			fxShader->bindAttrib(AT_TEXCOORD, "textureCoordinate");
			if( fxShader->link() ){
				fxShader->use();
				fxShader->setUniform1i("tex", 0);
			}else success = false;
		}else success = false;

		ShaderManager::ShaderProgram* terrain = shaderManager->makeShaderProgram("terrain", "shaders/terraintest.vsh", "shaders/terraintest.fsh");
		if( terrain ){
			terrain->bindAttrib(AT_TEXCOORD, "textureCoordinate");
			if( !terrain->link() ) success = false;
		}else success = false;

		//if( shaderManager->makeShaderProgram("TextureLightning", "shaders/texturelightning.vsh", "shaders/texturelightning.fsh") ){
		//	shaderManager->bindAttrib("TextureLightning", "textureCoordinate", AT_TEXCOORD);
		//	shaderManager->bindAttrib("TextureLightning", "position", AT_VERTEX);
		//	shaderManager->bindAttrib("TextureLightning", "color", AT_COLOR);
		//	shaderManager->bindAttrib("TextureLightning", "normal", AT_NORMAL);
		//	if( !shaderManager->linkProgram("TextureLightning") ) success = false;
		//}else success = false;

		//if( shaderManager->makeShaderProgram("Ortho", "shaders/orthogonal.vsh", "shaders/orthogonal.fsh") ){
		//	shaderManager->bindAttrib("Ortho", "textureCoordinate", AT_TEXCOORD);
		//	shaderManager->bindAttrib("Ortho", "position", AT_VERTEX);
		//	if( !shaderManager->linkProgram("Ortho") ) success = false;
		//}else success = false;
		
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
