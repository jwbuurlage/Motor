#include "MotorMeshManager.h"
#include "MotorMaterialManager.h"
#define _USE_MATH_DEFINES
#include <math.h>

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
	
	Mesh* MeshManager::loadResource( const char* filename ){
		return 0;
	}
    
    void MeshManager::loadPrimitives(){
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

		addResource("default", cubeMesh);
        
        //SPHERE
        
        Mesh* sphereMesh = new Mesh;
        
        /******************************************************************
         *																  *
         *			   --+--  			SPHERE, SLICED IN LINES LONGITUDE *
         *		     +-+-+-+-+			AND LATITUDE.					  *
         *		   +--+--+--+--+										  *
         *		  +---+--+--+---+		Quads are then sliced into two	  *
         *		  +---+--+--+---+		triangles.						  *
         *		  +---+--+--+---+ 										  *
         *		   +--+--+--+--+		then the vertices are returned	  *
         *		    +--+-+-+-+ 			and can be drawn with GL.TRIANGLE *
         *		       --+-- 			Texture coords are on by default  *
         *																  *
         *******************************************************************/
        
        const int s = 1; //size
        const int p = 30; //latslices
        const int q = 30; //lonslices
        int n = 0;
        float theta, theta2, phi, phi2; 
        GLfloat sphereVertices[ 72 * p * q ];
        
        //we want to move in steps M_PI/p, this is what we'll call phi. 
        //And for every step we want to loop through
        //all the q (2 M_PI / q)
        //we start at the bottom of the sphere
        
        for(int i = 0; i < p; ++i) {
            n = 72 * q * i; //30 per longslice
            //log(n);
            phi = i * (float)(M_PI/p);
            phi2 = (i + 1) * (float)(M_PI/p); 
            for(int j = 0; j < q; ++j) {
                theta = j * (float)(2*M_PI/q);
                theta2 = (j + 1) * (float)(2*M_PI/q);
                
                //vary between the current, and the following longitude and latitude.
                // 
                // (3) ------(4)
                // (1)|_____/(2)
                //
                //That's an example of a slice, we want to make to triangles
                //one defined by (1),(2),(3) and one by (2),(3),(4)
                //starting at the positive x-axis
                
                sphereVertices[n] = s * sin(phi) * cos(theta2); 
                sphereVertices[n+1] = s * sin(phi) * sin(theta2);
                sphereVertices[n+2] = s * cos(phi);	
                
                sphereVertices[n+3] = 1.0f;        //TEMPORARY COLOR AND NORMALS LIKE THIS
                sphereVertices[n+4] = 1.0f;
                sphereVertices[n+5] = 1.0f;					        
                sphereVertices[n+6] = 1.0f;
                
                sphereVertices[n+7] = sin(phi) * cos(theta2);
                sphereVertices[n+8] = sin(phi) * sin(theta2);					        
                sphereVertices[n+9] = cos(phi);
                
                sphereVertices[n+10] = (float)(j+1)/q;					   
                sphereVertices[n+11] = (float)i/p;									 
                
                sphereVertices[n+12] = s * sin(phi) * cos(theta);
                sphereVertices[n+13] = s * sin(phi) * sin(theta);
                sphereVertices[n+14] = s * cos(phi);	
                
                sphereVertices[n+15] = 1.0f;        //TEMPORARY COLOR AND NORMALS LIKE THIS
                sphereVertices[n+16] = 1.0f;
                sphereVertices[n+17] = 1.0f;					        
                sphereVertices[n+18] = 1.0f;
                
                sphereVertices[n+19] = sin(phi) * cos(theta);
                sphereVertices[n+20] = sin(phi) * sin(theta);					        
                sphereVertices[n+21] = cos(phi);
                
                sphereVertices[n+22] = (float)j/q;					   
                sphereVertices[n+23] = (float)i/p;		
                
                sphereVertices[n+24] = s * sin(phi2) * cos(theta); 	
                sphereVertices[n+25] = s * sin(phi2) * sin(theta); 	
                sphereVertices[n+26] = s * cos(phi2);		
                
                sphereVertices[n+27] = 1.0f;        //TEMPORARY COLOR AND NORMALS LIKE THIS
                sphereVertices[n+28] = 1.0f;
                sphereVertices[n+29] = 1.0f;					        
                sphereVertices[n+30] = 1.0f;
                
                sphereVertices[n+31] = sin(phi2) * cos(theta);
                sphereVertices[n+32] = sin(phi2) * sin(theta);					        
                sphereVertices[n+33] = cos(phi2);
                
                sphereVertices[n+34] = (float)j/q;					   
                sphereVertices[n+35] = (float)(i+1)/p;	
                
                sphereVertices[n+36] = s * sin(phi) * cos(theta2);  	
                sphereVertices[n+37] = s * sin(phi) * sin(theta2);  	
                sphereVertices[n+38] = s * cos(phi);			
                
                sphereVertices[n+39] = 1.0f;        //TEMPORARY COLOR AND NORMALS LIKE THIS
                sphereVertices[n+40] = 1.0f;
                sphereVertices[n+41] = 1.0f;					        
                sphereVertices[n+42] = 1.0f;
                
                sphereVertices[n+43] = sin(phi) * cos(theta2);
                sphereVertices[n+44] = sin(phi) * sin(theta2);					        
                sphereVertices[n+45] = cos(phi);
                
                sphereVertices[n+46] = (float)(j+1)/q;					   
                sphereVertices[n+47] = (float)i/p;									 
                
                sphereVertices[n+48] = s * sin(phi2) * cos(theta); 	
                sphereVertices[n+49] = s * sin(phi2) * sin(theta); 	
                sphereVertices[n+50] = s * cos(phi2);				
                
                sphereVertices[n+51] = 1.0f;        //TEMPORARY COLOR AND NORMALS LIKE THIS
                sphereVertices[n+52] = 1.0f;
                sphereVertices[n+53] = 1.0f;					        
                sphereVertices[n+54] = 1.0f;
                
                sphereVertices[n+55] = sin(phi2) * cos(theta);
                sphereVertices[n+56] = sin(phi2) * sin(theta);					        
                sphereVertices[n+57] = cos(phi2);
                
                sphereVertices[n+58] = (float)j/q;					   
                sphereVertices[n+59] = (float)(i+1)/p;									 
                
                sphereVertices[n+60] = s * sin(phi2) * cos(theta2); 
                sphereVertices[n+61] = s * sin(phi2) * sin(theta2); 	
                sphereVertices[n+62] = s * cos(phi2);				     
                
                sphereVertices[n+63] = 1.0f;        //TEMPORARY COLOR AND NORMALS LIKE THIS
                sphereVertices[n+64] = 1.0f;
                sphereVertices[n+65] = 1.0f;					        
                sphereVertices[n+66] = 1.0f;
                
                sphereVertices[n+67] = sin(phi2) * cos(theta2);
                sphereVertices[n+68] = sin(phi2) * sin(theta2);					        
                sphereVertices[n+69] = cos(phi2);
                
                sphereVertices[n+70] = (float)(j+1)/q;					   
                sphereVertices[n+71] = (float)(i+1)/p;	
                
                n += 72; //30 per longslice		
            }	
        }
        
        
        //-- MESH DATA AND VALUES --
        //generating and binding the vertex buffer
        glGenBuffers(1, &sphereMesh->vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, sphereMesh->vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertices), &sphereVertices, GL_STATIC_DRAW);
        
        //setting appropriate vertexbuffer values
        sphereMesh->hasColor = true;
        sphereMesh->hasNormal = true;
        sphereMesh->vertexCount = 6 * p * q;
        sphereMesh->vertexBufferSize = sizeof(sphereVertices);
        sphereMesh->vertexBufferDataType = GL_FLOAT;
        sphereMesh->primitiveType = GL_TRIANGLES;
        sphereMesh->dimension = 3;
        sphereMesh->stride = 48;
        sphereMesh->vertexOffset = 0;
        
        //generating and binding the index buffer
        /* glGenBuffers(1, &CubeMesh.indexBuffer);
         glBindBuffer(GL_ARRAY_BUFFER, CubeMesh.indexBuffer);
         glBufferData(GL_ARRAY_BUFFER, sizeof(cubeIndices), &cubeIndices, GL_STATIC_DRAW); */
        
        //setting appropriate indexbuffer values
        sphereMesh->hasIndexBuffer = false;
        addResource("sphere", sphereMesh);
        
        //PLANE    
        
        Mesh* planeMesh = new Mesh;

        static const GLfloat planeVertices[] = {
            //---  POSITION ---  -------- COLOR -------  ---- NORMAL ----    ---- TEXTURE ----
            20.0f,  0.0f,  20.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            20.0f,  0.0f, -20.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -20.0f,  0.0f, -20.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            20.0f,  0.0f,  20.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            -20.0f,  0.0f, -20.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -20.0f,  0.0f,  20.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
        };
        
        //-- MESH DATA AND VALUES --
        //generating and binding the vertex buffer
        glGenBuffers(1, &planeMesh->vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, planeMesh->vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
        
        //setting appropriate vertexbuffer values
        planeMesh->hasColor = true;
        planeMesh->hasNormal = true;
        planeMesh->vertexCount = 6;
        planeMesh->vertexBufferSize = sizeof(planeVertices);
        planeMesh->vertexBufferDataType = GL_FLOAT;
        planeMesh->primitiveType = GL_TRIANGLES;
        planeMesh->dimension = 3;
        planeMesh->stride = 48;
        planeMesh->vertexOffset = 0;
        
        //setting indexbuffer value to false
        planeMesh->hasIndexBuffer = false;
        addResource("plane", planeMesh);
       
    }

	void MeshManager::loadDefaultMesh(){
        loadPrimitives();
		return;
	}

}
