#include "MotorModelManager.h"
#include "MotorMeshManager.h"
#include "MotorMaterialManager.h"
#include "MotorLogger.h"

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
			addResource(newName, newModel);
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
		ModelHeader*	head;
        frame*          framePtr;
        triangle*       trianglePtr;
        texCoo*			texCooPtr;
        
        //Open the file
        std::string location;
        location += "models/";
        location += filename;
        location += ".md2";
		const File* modelfile = Filesystem::getSingleton().getFile(location.c_str());
        
        if(!modelfile){
			Logger::getSingleton().log(Logger::WARNING, "Could not load model file");
            return NULL;
		}
        
		head = (ModelHeader*)modelfile->data;

        // 1) Check if the model is an md2 file
        // 2) Check if it's md2 version 8 (we will require this)
        if( modelfile->size < sizeof(ModelHeader) || (head->identity != MD2_IDENTITY) || (head->version != MD2_VERSION)) {
            std::cout << "ERROR: " << filename << " is not a valid md2 model." << std::endl;
            return NULL;
        }
        
        // 3) Copy the header data into our classvariables        
        int frameSize = head->frameSize;
        int frameCount = head->nFrames;
        int triangleCount = head->nTriangles;
        
        if (frameCount > 1) {
            model->animated = true;
        }

        model->triangleCount = head->nTriangles;
        
        Mesh* modelMesh = new Mesh;
		

        char* buffer = modelfile->data + head->oFrames; //size = frameSize * frameCount
 
        trianglePtr = (triangle*)(modelfile->data + head->oTriangles); //size = sizeof(triangle) * triangleCount * frameCount
        texCooPtr = (texCoo*)(modelfile->data + head->oTexCoo); //size = sizeof(texCoo) * head.nTexCoo; 
                
        // 6) Create our VBO and elements object.
		GLfloat* vertices = new GLfloat[(triangleCount * 3) * 12 * frameCount];

        for( int j = 0; j < frameCount; j++ )
        {
            //we need to adjust our pointer every loop (frame)
            framePtr = (frame*)&buffer[j * frameSize];
            
            for( int k = 0; k < triangleCount; k++ )
            {
                for(int m = 0; m < 3; m++) {
                    int index = trianglePtr[k].vert[m];
                    int texcooindex = trianglePtr[k].tex[m];
                    
                    int frameOffset = (j * (triangleCount * 3) * 12);
                    int triangleOffset = (12 * (3*k + m));
                    
                    //vertices
                    vertices[frameOffset + triangleOffset + 0] = (framePtr->verts[index].v[1] * framePtr->scale[1]) + framePtr->translate[1];
                    vertices[frameOffset + triangleOffset + 1] = (framePtr->verts[index].v[2] * framePtr->scale[2]) + framePtr->translate[2];
                    vertices[frameOffset + triangleOffset + 2] = -((framePtr->verts[index].v[0] * framePtr->scale[0]) + framePtr->translate[0]);
                    
                    //color
                    vertices[frameOffset + triangleOffset + 3] = 1.0f;
                    vertices[frameOffset + triangleOffset + 4] = 1.0f;
                    vertices[frameOffset + triangleOffset + 5] = 1.0f;
                    vertices[frameOffset + triangleOffset + 6] = 1.0f;

                    //normals
                    vertices[frameOffset + triangleOffset + 7] = model->anorms[framePtr->verts[index].lightnormalindex][1];
                    vertices[frameOffset + triangleOffset + 8] = model->anorms[framePtr->verts[index].lightnormalindex][2];
                    vertices[frameOffset + triangleOffset + 9] = -model->anorms[framePtr->verts[index].lightnormalindex][0];
                    
                    //texcoo
                    vertices[frameOffset + triangleOffset + 10] = (float)texCooPtr[texcooindex].s / head->textureWidth;
                    vertices[frameOffset + triangleOffset + 11] = ((float)texCooPtr[texcooindex].t / head->textureHeight);
                }
            }
            
        }
        
		// 7) Bind these buffers
		glGenBuffers(1, &modelMesh->vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, modelMesh->vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * triangleCount * 3 * 12 * frameCount, vertices, GL_STATIC_DRAW);

		delete[] vertices;

        modelMesh->hasColor = true;
        modelMesh->hasNormal = true;
        modelMesh->hasIndexBuffer = false;
        modelMesh->vertexCount = triangleCount * 3;
        modelMesh->vertexBufferSize = sizeof(GLfloat) * triangleCount * 3 * 12;
        modelMesh->vertexBufferDataType = GL_FLOAT;
        modelMesh->primitiveType = GL_TRIANGLES;
        modelMesh->dimension = 3;
        modelMesh->stride = 48;
        modelMesh->vertexOffset = 0;
        
        model->setMesh(modelMesh);
        
        std::string location_texture;
        location_texture += "textures/";
        location_texture += filename;
        location_texture += ".tga";
        model->setMaterial( MaterialManager::getSingleton().getMaterial(location_texture.c_str()) );
        
        // delete file
        Filesystem::getSingleton().unloadFile(modelfile);

		addResource(filename, model);
        
        return getResource(filename);;
    }
    
    Model* ModelManager::loadCOLLADA( const char* filename ){
        //we will load the vertex data before anything else.
        //use tinyxml to extract data
    
        return 0;
    }

	Model* ModelManager::loadResource( const char* filename ){
        //for now only MD2 is supported
		return loadMD2(filename);;
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
