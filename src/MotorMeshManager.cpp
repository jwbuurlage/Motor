#include "MotorMeshManager.h"
#include "MotorMaterialManager.h"

namespace Motor {

	template<> MeshManager* Singleton<MeshManager>::singleton = 0;

	MeshManager::MeshManager(){
	}

	MeshManager::~MeshManager(){
		cleanup();
	}

	int MeshManager::initialize(){
		loadDefaultMesh();
		return 1;
	}

	void MeshManager::cleanup(){
		unloadAll();
	}
	
	const Mesh* MeshManager::loadResource( const char* filename ){
		return 0;
	}

	void MeshManager::loadDefaultMesh(){
		if( resourceLoaded("default") ) return;

		Mesh* cubeMesh = new Mesh;

		//-----------------------------
		//--- VERTEX AND INDEX DATA ---
		//                            -
		//         2--3               -
		//     6--7|  |               -
		//     |  ||  |               -
		//     |  |0--1               -
		//     4--5                   -
		//                            -
		//-----------------------------
		static const GLfloat cubeVertexData[] = {
			//---  POSITION ---  -------- COLOR -------         ---- NORMAL ----      ---- TEX ----
			-1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f, -1.0f,  0.0f, 0.0f,    //0
			-1.0f,  1.0f, -1.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f, -1.0f,  0.0f, 1.0f,    //2
			1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f, -1.0f,  1.0f, 0.0f,    //1
			-1.0f,  1.0f, -1.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f, -1.0f,  0.0f, 1.0f,    //2
			1.0f,  1.0f, -1.0f,     1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f, -1.0f,  1.0f, 1.0f,     //3
			1.0f, -1.0f, -1.0f,     1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f, -1.0f,  1.0f, 0.0f,     //1
        
			1.0f,  1.0f, -1.0f,     1.0f, 1.0f, 1.0f, 1.0f,     0.0f,  1.0f, 0.0f,  0.0f, 0.0f,     //3
			-1.0f,  1.0f, -1.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f,  1.0f, 0.0f,  1.0f, 0.0f,     //2
			1.0f,  1.0f,  1.0f,     1.0f, 1.0f, 1.0f, 1.0f,     0.0f,  1.0f, 0.0f,  0.0f, 1.0f,     //7
			-1.0f,  1.0f, -1.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f,  1.0f, 0.0f,  1.0f, 0.0f,     //2
			-1.0f,  1.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f,  1.0f, 0.0f,  1.0f, 1.0f,     //6
			1.0f,  1.0f,  1.0f,     1.0f, 1.0f, 1.0f, 1.0f,     0.0f,  1.0f, 0.0f,  0.0f, 1.0f,     //7
        
			-1.0f,  1.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,     -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,    //6
			-1.0f,  1.0f, -1.0f,    1.0f, 1.0f, 1.0f, 1.0f,     -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,    //2
			-1.0f, -1.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,     -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,    //4
			-1.0f,  1.0f, -1.0f,    1.0f, 1.0f, 1.0f, 1.0f,     -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,    //2
			-1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 1.0f, 1.0f,     -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,    //0
			-1.0f, -1.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,     -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,    //4
        
			-1.0f, -1.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f,    //4
			1.0f, -1.0f,  1.0f,     1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f, 1.0f,   1.0f, 0.0f,     //5
			-1.0f,  1.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f, 1.0f,   0.0f, 1.0f,    //6
			-1.0f,  1.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f, 1.0f,   0.0f, 1.0f,    //6
			1.0f, -1.0f,  1.0f,     1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f, 1.0f,   1.0f, 0.0f,     //5
			1.0f,  1.0f,  1.0f,     1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 0.0f, 1.0f,   1.0f, 1.0f,     //7
        
			1.0f,  1.0f,  1.0f,     1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f, 0.0f,   0.0f, 1.0f,     //7
			1.0f, -1.0f,  1.0f,     1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f, 0.0f,   0.0f, 0.0f,     //5
			1.0f, -1.0f, -1.0f,     1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f, 0.0f,   1.0f, 0.0f,     //1
			1.0f,  1.0f,  1.0f,     1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f, 0.0f,   0.0f, 1.0f,     //7
			1.0f, -1.0f, -1.0f,     1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f, 0.0f,   1.0f, 0.0f,     //1
			1.0f,  1.0f, -1.0f,     1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f, 0.0f,   1.0f, 1.0f,     //3
        
			-1.0f, -1.0f,  1.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, -1.0f, 0.0f,  0.0f, 0.0f,    //4
			-1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, -1.0f, 0.0f,  0.0f, 1.0f,    //0
			1.0f, -1.0f,  1.0f,     1.0f, 1.0f, 1.0f, 1.0f,     0.0f, -1.0f, 0.0f,  1.0f, 0.0f,     //5
			-1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 1.0f, 1.0f,     0.0f, -1.0f, 0.0f,  0.0f, 1.0f,    //0
			1.0f, -1.0f, -1.0f,     1.0f, 1.0f, 1.0f, 1.0f,     0.0f, -1.0f, 0.0f,  1.0f, 1.0f,      //1
			1.0f, -1.0f,  1.0f,     1.0f, 1.0f, 1.0f, 1.0f,     0.0f, -1.0f, 0.0f,  1.0f, 0.0f     //5
		};

		//generating and binding the opengl buffer
		glGenBuffers(1, &cubeMesh->vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, cubeMesh->vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexData), &cubeVertexData, GL_STATIC_DRAW);

		//Set the appropriate values in the Mesh object
		cubeMesh->hasColor = true;
		cubeMesh->hasNormal = true;
		cubeMesh->vertexCount = 36;
		cubeMesh->vertexBufferSize = sizeof(cubeVertexData);
		cubeMesh->vertexBufferDataType = GL_FLOAT;
		cubeMesh->primitiveType = GL_TRIANGLES;
		cubeMesh->dimension = 3;
		cubeMesh->stride = 48;
		cubeMesh->vertexOffset = 0;

		cubeMesh->hasIndexBuffer = false;
		cubeMesh->material = MaterialManager::getSingleton().getMaterial("default");

		addResource("default", cubeMesh);

		return;
	}

}
