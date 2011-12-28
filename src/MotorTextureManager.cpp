#include "MotorTextureManager.h"
#include "MotorLogger.h"
#include <sstream>

namespace Motor {

	template<> TextureManager* Singleton<TextureManager>::singleton = 0;

	TextureManager::TextureManager(){
	}

	TextureManager::~TextureManager(){
		cleanup();
	}

	int TextureManager::initialize(){
		loadDefaultTexture();
		return 1;
	}

	void TextureManager::cleanup(){

	}

	const Texture* TextureManager::loadResource( const char* filename ){
		return 0;
	}

	void TextureManager::loadDefaultTexture(){
		if( resourceLoaded("default") ) return;

		Texture* defaultTex = new Texture;
		defaultTex->handle = 0;
		defaultTex->width = 4;
		defaultTex->height = 4;

		unsigned int pixelCount = defaultTex->width * defaultTex->height;
		float* imageData = new float[pixelCount * 4];
		for(unsigned int i = 0; i < pixelCount; ++i){
			imageData[4*i+0] = 0.2f; //red
			imageData[4*i+1] = 0.2f; //green
			imageData[4*i+2] = 0.9f; //blue
			imageData[4*i+3] = 0.8f; //alpha
		}

		glGetError();
		glGenTextures(1, &defaultTex->handle);
		glBindTexture( GL_TEXTURE_2D, defaultTex->handle );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, defaultTex->width, defaultTex->height, 0, GL_RGBA, GL_FLOAT, imageData );

		delete[] imageData;

		addResource("default", defaultTex);
		return;
	}
}
