#pragma once
#include "MotorSingleton.h"
#include "MotorResourceManager.h"
#include "MotorTexture.h"

namespace Motor {

	class TextureManager : public Singleton<TextureManager>, public ResourceManager<Texture> {
	public:
		TextureManager();
		~TextureManager();

		int initialize();
		void cleanup();

		const Texture* getTexture( const char* filename ){ return getResource(filename); }

	private:
		const Texture* loadResource( const char* filename );

		void loadDefaultTexture(); //Generates default texture
	};
}