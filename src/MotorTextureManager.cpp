#include "MotorTextureManager.h"
#include "MotorLogger.h"
#include "MotorFilesystem.h"
#include <sstream>
#include <SFML/Graphics.hpp>

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

	Texture* TextureManager::loadResource( const char* filename ){
		const File* imagefile = Filesystem::getSingleton().getFile(filename);
		if( imagefile == 0 ) return 0;
		Texture* texture = 0;
		sf::Image image;
		if( image.LoadFromMemory(imagefile->data, imagefile->size) ){

			texture = new Texture;
			texture->width = image.GetWidth();
			texture->height = image.GetHeight();

			glGenTextures(1, &texture->handle);
			glBindTexture( GL_TEXTURE_2D, texture->handle );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetPixelsPtr() );

			addResource(filename, texture);
		}
		//since the sf::Image object is destroyed, it will unload the image data. OpenGL will have it now.
		return texture;
	}

	void TextureManager::loadDefaultTexture(){
		if( resourceLoaded("default") ) return;

		/* Texture* defaultTex = new Texture;
		defaultTex->handle = 0;
		defaultTex->width = 4;
		defaultTex->height = 4;

		unsigned int pixelCount = defaultTex->width * defaultTex->height;
		float* imageData = new float[pixelCount * 4];
		for(unsigned int i = 0; i < pixelCount; ++i){
			imageData[4*i+0] = 0.6f*i/pixelCount+0.4f; //red
			imageData[4*i+1] = 0.6f*1/pixelCount; //green
			imageData[4*i+2] = 0.8f*i/pixelCount+0.2f; //blue
			imageData[4*i+3] = 0.8f; //alpha
		}

		//glGetError();
		glGenTextures(1, &defaultTex->handle);
		glBindTexture( GL_TEXTURE_2D, defaultTex->handle );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, defaultTex->width, defaultTex->height, 0, GL_RGBA, GL_FLOAT, imageData );

		delete[] imageData; */

		addResource("default", getResource("textures/stone.tga"));
		return;
	}
}
