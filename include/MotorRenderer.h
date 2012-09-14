#pragma once
#include "matrix.h"
#include "MotorScene.h" //For ObjectContainer definitions
#include <GL/glew.h>

namespace Motor {
	class ShaderManager;
    class Terrain;

	class Renderer {
	public:
		Renderer();
		~Renderer();
		
		int initialize(int width, int height);
		void cleanup();

		void setWindowSize(int width, int height);

		bool renderFrame();

		//returns false if an OpenGL error has occured since previous check
		//this function will log the error code
		bool checkErrors();

		//Can be 0 pointer if no objects/effects/lights
		void setObjectList(ObjectContainer* _objects){ objects = _objects; };
		void setEffectList(EffectContainer* _effects){ effects = _effects; };
		void setLightList(LightContainer* _lights){ lights = _lights; };
        void setTerrain(Terrain* _terrain) { terrain = _terrain; };
        
		mat* viewMatrixPtr(){ return &viewMatrix; };

	private:
		bool initialized;
		int windowWidth, windowHeight;
		ShaderManager* shaderManager;

		void drawObject(SceneObject* obj, bool depthOnly);

		void drawParticleEffect(ParticleEffect* fx);
		//TODO: make this better
		GLfloat* particleBuffer;
		int particleBufferSize;
		
        Terrain* terrain;
		void drawTerrain();
		void initTerrain();
		GLuint terrainVertexBuffer;
		GLuint terrainIndexBuffer;
		GLsizei terrainVertexCount;
		GLsizei terrainIndexCount;
        
		//TODO: the application should decide what shaders to load
		//Expose ShaderManager in a nice way to application
		bool loadShaders();

		//Matrices that are the same for all frames are saved
		mat projectionMatrix;
        mat projectionMatrixShadow;
		mat viewMatrix;
		mat projViewMatrix;
        mat lightProjViewMatrix;
        mat biasMatrix;
		void generateProjectionMatrix();
        
        ////////// SHADOW SUPPORT ///////////
        void setUpShadowFBO();
        GLuint shadowTexMapHandle;
        GLuint shadowFBOHandle;

		ObjectContainer* objects;
		EffectContainer* effects;
		LightContainer* lights;
        

		//Graphical settings
		bool shadowsEnabled;
	};

}
