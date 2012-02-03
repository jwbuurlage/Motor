#pragma once
#include "MotorSingleton.h"
#include "MotorResourceManager.h"
#include "MotorMesh.h"

namespace Motor {

	class MeshManager : public Singleton<MeshManager>, public ResourceManager<Mesh> {
	public:
		MeshManager();
		~MeshManager();

		int initialize();
		void cleanup();

		Mesh* getMesh( const char * filename ){ return getResource(filename); }

	private:
		Mesh* loadResource( const char* filename );

        void loadPrimitives(); 
		void loadDefaultMesh(); //generates default mesh for when file can not be found
	};

}