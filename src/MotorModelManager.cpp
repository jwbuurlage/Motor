#include "MotorModelManager.h"
#include "MotorMeshManager.h"
#include "MotorMaterialManager.h"

#include <iostream>

#define MD2_IDENTITY (('2'<<24) + ('P'<<16) + ('D'<<8) + 'I')
#define MD2_VERSION 8

namespace Motor {
    
vec3 MD2Model::anorms[] = {
#include "anorms.h"
}; 


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
    
    Model* ModelManager::loadMD2( const char* filename ){
       	// The md2 file format is specified as follows
        //              ___________
        //			   |	HEAD   |
        //				-----|-----
        //				_____|_____
        //			   |	DATA   |
        //				-----------
        // 
        //	Head is of fixed size, and is defined in the ModelHeader struct
        //	Data contains lists of texnames, texcoordinates, triangles, frames (vertices), 
        //	and GLCommands.
        //
        //	We will first define pointers so that we can dynamically load in memory (because
        //	of varying model-sizes), and after that we will:
        //		1) Check if the model is an md2 file
        //		2) Check if it's md2 version 8 (we will require this)
        //		3) Copy the header data into our classvariables
        //		4) Allocate the memory for our buffers and VBO
        //		5) Load in our buffers
        //		6) Create our VBO and elements object.
        //		7) Bind these buffers
        //		8) Clean up, release data

        MD2Model* model = new MD2Model;
        
        //Define pointers and other variables
        ModelHeader     head;
        //pointers
        char*           buffer;
        char*           triangleBuffer;
        char*           texCooBuffer;
        frame*          framePtr;
        triangle*       trianglePtr;
        texCoo*			texCooPtr;
                
        //Open the file
		const File* modelfile = Filesystem::getSingleton().getFile(filename);
        
        if(!modelfile->data)
            return NULL;
        
        //Read the header
        memcpy(&head, modelfile->data, sizeof(ModelHeader));        
        
        // 1) Check if the model is an md2 file
        // 2) Check if it's md2 version 8 (we will require this)
        if((head.identity != MD2_IDENTITY) || (head.version != MD2_VERSION)) {
            std::cout << "ERROR: " << filename << " is not a valid md2 model." << std::endl;
            return NULL;
        }
        
        std::cout << "LOADED: " << filename << " , an awesome model." << head.version << std::endl;
        
        // 3) Copy the header data into our classvariables        
        int frameSize = head.frameSize;
        int frameCount = head.nFrames;
        int triangleCount = head.nTriangles;
        
        Mesh* modelMesh = new Mesh;
        model->setMesh(modelMesh);
        model->getMesh()->vertexCount = model->triangleCount * 3;

        std::cout << "INFO: " << head.oFrames << std::endl;
        
        memcpy(&texCooBuffer, modelfile->data, frameSize * frameCount);
        memcpy(&texCooBuffer, modelfile->data + head.oTexCoo, sizeof(texCoo) * head.nTexCoo);
        memcpy(&triangleBuffer, modelfile->data + head.oTriangles, sizeof(triangle) * triangleCount * frameCount);
 
        trianglePtr = (triangle*)triangleBuffer;
        texCooPtr = (texCoo*)texCooBuffer; 
                
        // 6) Create our VBO and elements object.
		GLfloat* vertices = new GLfloat[(triangleCount * 3) * 8];
        
		framePtr = (frame*)&buffer;
        
		for( int k = 0; k < triangleCount; k++ )
		{
			for(int m = 0; m < 3; m++) {
				int index = trianglePtr[k].vert[m];
				int texcooindex = trianglePtr[k].tex[m];
                
				//vertices
				vertices[(8 * (3*k + m)) + 0] = (framePtr->verts[index].v[1] * framePtr->scale[1]) + framePtr->translate[1];
				vertices[(8 * (3*k + m)) + 1] = (framePtr->verts[index].v[2] * framePtr->scale[2]) + framePtr->translate[2];
				vertices[(8 * (3*k + m)) + 2] = -((framePtr->verts[index].v[0] * framePtr->scale[0]) + framePtr->translate[0]);
                
				//normals
				vertices[(8 * (3*k + m)) + 3] = model->anorms[framePtr->verts[index].lightnormalindex][1];
				vertices[(8 * (3*k + m)) + 4] = model->anorms[framePtr->verts[index].lightnormalindex][2];
				vertices[(8 * (3*k + m)) + 5] = -model->anorms[framePtr->verts[index].lightnormalindex][0];
                
				//texcoo
				vertices[(8 * (3*k + m)) + 6] = (float)texCooPtr[texcooindex].s / head.textureWidth;
				vertices[(8 * (3*k + m)) + 7] = 1-((float)texCooPtr[texcooindex].t / head.textureHeight);
			}
		}
        
		// 7) Bind these buffers
		glGenBuffers(1, &modelMesh->vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, modelMesh->vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * (model->triangleCount * 3) * 8, vertices, GL_STATIC_DRAW);


        // 8) Clean up, release data
        delete [] buffer;
        delete [] triangleBuffer;
        delete [] texCooBuffer;

		addResource(filename, model);
        
        return model;
    }
    
    Model* ModelManager::loadCOLLADA( const char* filename ){
        //we will load the vertex data before anything else.
        //use tinyxml to extract data
    
        return 0;
    }

	Model* ModelManager::loadResource( const char* filename ){
        //for now only MD2 is supported
        loadMD2(filename);
        
		return 0;
	}

	void ModelManager::loadDefaultModel(){
		if( resourceLoaded("default") ) return;

		Model* cubeModel = new Model;
		cubeModel->setMesh( MeshManager::getSingleton().getMesh("default") );
		cubeModel->setMaterial( MaterialManager::getSingleton().getMaterial("default") );
		addResource("default", cubeModel);
                
        Model* sphereModel = new Model;
		sphereModel->setMesh( MeshManager::getSingleton().getMesh("sphere") );
		sphereModel->setMaterial( MaterialManager::getSingleton().getMaterial("default") );
        
        Model* planeModel = new Model;
		planeModel->setMesh( MeshManager::getSingleton().getMesh("plane") );
		planeModel->setMaterial( MaterialManager::getSingleton().getMaterial("default") );
        
		addResource("sphere", sphereModel);
		addResource("plane", planeModel);

	}

}