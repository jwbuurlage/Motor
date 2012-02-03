#include "MotorModelManager.h"
#include "MotorMeshManager.h"
#include "MotorMaterialManager.h"

namespace Motor {

	template<> ModelManager* Singleton<ModelManager>::singleton = 0;

	ModelManager::ModelManager(){
	}

	ModelManager::~ModelManager(){
		cleanup();
	}

	int ModelManager::initialize(){
		loadDefaultModel();
		return 1;
	}

	void ModelManager::cleanup(){
		unloadAll();
	}
	
	Model* ModelManager::createModelCopy( const char* filename, const char* newName ){
		if( resourceLoaded(filename) == false ){
			return getResource(newName);
		}else{
			Model* model = getResource(filename);
			Model* newModel = new Model;
			newModel->setMesh( model->getMesh() );
			newModel->setMaterial( model->getMaterial() );
			return newModel;
		}
	}

	Model* ModelManager::loadResource( const char* filename ){
		return 0;
	}

	void ModelManager::loadDefaultModel(){
		if( resourceLoaded("default") ) return;

		Model* cubeModel = new Model;
		cubeModel->setMesh( MeshManager::getSingleton().getMesh("default") );
		cubeModel->setMaterial( MaterialManager::getSingleton().getMaterial("default") );
		addResource("default", cubeModel);
<<<<<<< HEAD
        
        Model* sphereModel = new Model;
		sphereModel->setMesh( MeshManager::getSingleton().getMesh("sphere") );
		sphereModel->setMaterial( MaterialManager::getSingleton().getMaterial("default") );
		addResource("sphere", sphereModel);

        Model* planeModel = new Model;
		planeModel->setMesh( MeshManager::getSingleton().getMesh("plane") );
		planeModel->setMaterial( MaterialManager::getSingleton().getMaterial("default") );
		addResource("plane", planeModel);
=======
>>>>>>> 9117291af14eb1668182493b8d49252f6c4bdc9c
	}

}