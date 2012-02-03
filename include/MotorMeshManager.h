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
<<<<<<< HEAD
        void loadPrimitives(); 
=======
>>>>>>> 9117291af14eb1668182493b8d49252f6c4bdc9c
		Mesh* loadResource( const char* filename );

		void loadDefaultMesh(); //generates default mesh for when file can not be found
	};

}