#pragma once
#include "MotorSingleton.h"
#include "MotorResourceManager.h"
#include "MotorModel.h"
#include "MotorMD2Model.h"
#include "MotorFilesystem.h"

namespace Motor {

	class ModelManager : public Singleton<ModelManager>, public ResourceManager<Model> {
	public:
		ModelManager();
		~ModelManager();

		int initialize();
		void cleanup();

		Model* getModel( const char * filename ){ return getResource(filename); }

		//If the model is loaded, it makes a new instance
		//If not loaded, it loads a model with name newName
		//Usefull for having two models with the same mesh but different materials
		Model* createModelCopy( const char* filename, const char* newName );

	private:
		Model* loadResource( const char* filename );
        Model* loadMD2( const char* filename );
        Model* loadCOLLADA( const char* filename );
		void loadDefaultModel();
	};
}