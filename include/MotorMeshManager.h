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

		const Mesh* getMesh( const char * filename ){ return getResource(filename); }

	private:
		const Mesh* loadResource( const char* filename );
        
        void loadPrimitives(); 
		void loadDefaultMesh(); //generates default mesh for when file can not be found
	};
}