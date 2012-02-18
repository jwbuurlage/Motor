//
//  MotorTerrain.cpp
//  Motor
//
//  Created by Jan-Willem Buurlage on 10-02-12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "MotorTerrain.h"
#include "MotorLogger.h"
#include "MotorTextureManager.h"

//debug
#include <iostream>

namespace Motor {
 
    void Terrain::generate(float w_terrain, float l_terrain, float h_terrain) {
        
        //----------------------
        // make the scale matrix
        //----------------------
        
        scaleMatrix.scaleX(w_terrain).scaleZ(l_terrain).scaleY(h_terrain);
        scaleMatrix.translate(-w_terrain/2, 0.0f, -l_terrain/2);
      
        //-----------------------------------------
        // check if the heightmap is of proper size
        //-----------------------------------------

        int w, l;
        w = heightMap->width;
        l = heightMap->height;
        
        if(w != l || w != 257) {
            Logger::getSingleton().log(Logger::WARNING, "Heightmap is of wrong dimension.");
        }
                
        //-------------------------------------
        // prepare patches, and patch variables
        //-------------------------------------
           
        PATCH_COUNT_X = 8;
        PATCH_COUNT_Y = 8;
        PATCH_WIDTH = 33;
        PATCH_HEIGHT = 33;

        patches = new Patch[PATCH_COUNT_X*PATCH_COUNT_Y];
        
        //------------------------------------------------
        // prepare arrays which will hold VBO and IBO data
        //------------------------------------------------
        
        indexCount = (PATCH_HEIGHT-1) * (2*PATCH_WIDTH+1);
        
        GLfloat* vertices = new GLfloat[PATCH_WIDTH * PATCH_HEIGHT * 2];
        GLuint* indices = new GLuint[indexCount];

        float perVertex = 1 / (float)(PATCH_COUNT_X * PATCH_WIDTH);
        
        for(int i = 0; i < PATCH_WIDTH; ++i) {
            for(int j = 0; j < PATCH_HEIGHT; ++j) {
                //vertex
                int offset = ((i * PATCH_HEIGHT) + j) * 2;
                
                vertices[offset + 0] = (GLfloat)(i * perVertex); //x 
                vertices[offset + 1] = (GLfloat)(j * perVertex); //z
            }
        }
                
        int direction = -1;
        int offset = 0;
        int a, b;
        
        for(int i = 0; i < PATCH_WIDTH; ++i) { //w = patch_width
            direction *= -1;
            
            if(i % 2 == 0) {
                //dir ---->
                indices[(i+1)*(2*PATCH_WIDTH+1)-1] = (i+2)*PATCH_WIDTH - 1;
                offset = i*(2*PATCH_WIDTH + 1); 
                a = 0;
                b = 1;
            }
            else {
                //dir <----
                indices[(i+1)*(2*PATCH_WIDTH+1)-1] = (i+1)*PATCH_WIDTH;
                offset = (2*PATCH_WIDTH + 1)*(i+1) - 2; 
                a = 1;
                b = 0;
            }
            
            for(int j = 0; j < PATCH_HEIGHT - 1; ++j) { //l = patch_height
                //odd or even?
                indices[offset + direction*(j*2 + 0)] = ((i + a) * PATCH_HEIGHT + j);
                indices[offset + direction*(j*2 + 1)] = ((i + b) * PATCH_HEIGHT + j);
            }
        }
        
        for(int i = 0; i < (PATCH_COUNT_X * PATCH_COUNT_Y); ++i) {
            Patch thePatch;
            thePatch.lod = 0;
            thePatch.offset[0] = (i % PATCH_COUNT_X) * (1 / (float)PATCH_COUNT_X);
            thePatch.offset[1] = (floorf(i / PATCH_COUNT_Y)) * (1 / (float)PATCH_COUNT_Y);
            patches[i] = thePatch;
        }
        
        
        
        //----------------------------------
        // set up VBO and IBO, set variables
        //----------------------------------
                
        //generating and binding the opengl buffer
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * PATCH_WIDTH * PATCH_HEIGHT * 2, vertices, GL_STATIC_DRAW);
        
        glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_UNSIGNED_INT) * indexCount, indices, GL_STATIC_DRAW);
                
        delete vertices;
        delete indices;
    }
    
    void Terrain::updatePatches(Vector3 cameraPosition) {
        //calculate distance
        
        //set proper LOD (for now just preset distances)
        
    }
    
    void Terrain::draw(ShaderManager* shaderManager) {
        //loop through patches and draw them
    }
    
    float Terrain::heightAtPosition(int x, int y) {
        return 0.0f;
    }
}