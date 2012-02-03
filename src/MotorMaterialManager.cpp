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

	Material* MaterialManager::loadResource( const char* filename ){
		Material* mat = new Material;
		//TODO: actually load a material file which holds references to the different
		//textures that belong to it
		mat->texture = TextureManager::getSingleton().getTexture(filename);
		mat->color[0] = 1.0f;
		mat->color[1] = 1.0f;
		mat->color[2] = 1.0f;
		mat->color[3] = 1.0f;
		addResource(filename, mat);
		return mat;
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
