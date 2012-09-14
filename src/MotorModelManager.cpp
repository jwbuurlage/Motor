#include "MotorModelManager.h"
#include "MotorMD2Model.h"
#include "MotorColladaLoader.h"
#include "MotorMeshManager.h"
#include "MotorMaterialManager.h"
#include "MotorLogger.h"
#include <algorithm>


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
	
    Model* ModelManager::loadCOLLADA( const char* filename ){
		const File* modelfile = Filesystem::getSingleton().getFile(filename);
		if( modelfile == 0 ){
			LOG_WARNING("Could not find Collada file: " << filename);
            return NULL;
		}

		ColladaLoader loader;
		Model* model = loader.loadModel(filename, modelfile->data);
		if( model ) addResource(filename, model);

		Filesystem::getSingleton().unloadFile(modelfile);

        return model;
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
			Logger::getSingleton() << Logger::WARNING << "Could not load modelfile: " << filename << endLog;
            return NULL;
		}
        
		head = (ModelHeader*)modelfile->data;

        // 1) Check if the model is an md2 file
        // 2) Check if it's md2 version 8 (we will require this)
        if( modelfile->size < sizeof(ModelHeader) || (head->identity != MD2_IDENTITY) || (head->version != MD2_VERSION)) {
			Logger::getSingleton() << Logger::WARNING << "File is not a valid MD2 model file: " << filename << endLog;
			Filesystem::getSingleton().unloadFile(modelfile);
            return NULL;
        }
        
        MD2Model* model = new MD2Model;
        
        // 3) Copy the header data into our classvariables        
		model->frameCount = head->nFrames;
		model->frameSize = head->frameSize;
		model->triangleCount = head->nTriangles;
        
        if(model->frameCount > 1){
            model->animated = true;
        }

        model->triangleCount = head->nTriangles;
        
        Mesh* modelMesh = new Mesh;
		

        char* buffer = modelfile->data + head->oFrames; //size = frameSize * frameCount
 
        trianglePtr = (triangle*)(modelfile->data + head->oTriangles); //size = sizeof(triangle) * triangleCount * frameCount
        texCooPtr = (texCoo*)(modelfile->data + head->oTexCoo); //size = sizeof(texCoo) * head.nTexCoo; 
                
        // 6) Create our VBO and elements object.
		GLfloat* vertices = new GLfloat[(model->triangleCount * 3) * 12 * model->frameCount];

        for( int j = 0; j < model->frameCount; j++ )
        {
            //we need to adjust our pointer every loop (frame)
            framePtr = (frame*)&buffer[j * model->frameSize];
			int frameOffset = (j * (model->triangleCount * 3) * 12);
           
            for( int k = 0; k < model->triangleCount; k++ )
            {
                for(int m = 0; m < 3; m++) {
                    int index = trianglePtr[k].vert[m];
                    int texcooindex = trianglePtr[k].tex[m];

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
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model->triangleCount * 3 * 12 * model->frameCount, vertices, GL_STATIC_DRAW);

		delete[] vertices;

        modelMesh->hasColor = true;
        modelMesh->hasNormal = true;
        modelMesh->hasIndexBuffer = false;
        modelMesh->vertexCount = model->triangleCount * 3;
        modelMesh->vertexBufferSize = sizeof(GLfloat) * model->triangleCount * 3 * 12;
        //modelMesh->vertexBufferDataType = GL_FLOAT;
        modelMesh->primitiveType = GL_TRIANGLES;
        modelMesh->dimension = 3;
        modelMesh->stride = 48;
        //modelMesh->vertexOffset = 0;
        
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

	Model* ModelManager::loadResource( const char* filename ){
        //Find file type
		enum{ TYPE_UNKOWN, TYPE_MD2, TYPE_COLLADA } filetype = TYPE_UNKOWN;
		std::string name(filename);
		size_t found = name.find_last_of('.');
		if( found != std::string::npos ){
			name = name.substr(found+1);
			std::transform(name.begin(), name.end(), name.begin(), tolower);
			if( name == "dae" ){
				filetype = TYPE_COLLADA;
			}else if( name == "md2" ){
				filetype = TYPE_MD2;
			}
		}
		if( filetype == TYPE_COLLADA )
			return loadCOLLADA(filename);
		else
			return loadMD2(filename);
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
