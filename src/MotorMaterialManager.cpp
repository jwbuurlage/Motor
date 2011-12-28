#include "MotorMaterialManager.h"
#include "MotorTextureManager.h"
#include <GL/glew.h>

namespace Motor{

	template<> MaterialManager* Singleton<MaterialManager>::singleton = 0;

	MaterialManager::MaterialManager(){
	}

	MaterialManager::~MaterialManager(){
		cleanup();
	}

	int MaterialManager::initialize(){
		loadDefaultMaterial();
		return 1;
	}

	void MaterialManager::cleanup(){
		unloadAll();
	}

	const Material* MaterialManager::loadResource( const char* filename ){
		return 0;
	}

	void MaterialManager::loadDefaultMaterial(){
		if( resourceLoaded("default") ) return;
		Material* defaultMat = new Material;
		defaultMat->texture = TextureManager::getSingleton().getTexture("default");
		defaultMat->color[0] = 1.0f;
		defaultMat->color[1] = 1.0f;
		defaultMat->color[2] = 1.0f;
		defaultMat->color[3] = 1.0f;
		addResource("default", defaultMat);
		return;
	}

}
