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
        
        scaleMatrix.translate(-0.5f, 0.0f, -0.5f);
        scaleMatrix.scale(w_terrain);
      
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
        PATCH_WIDTH = 32;
        PATCH_HEIGHT = 32;

        patches = new Patch[PATCH_COUNT_X*PATCH_COUNT_Y];
        
        //------------------------------------------------
        // prepare arrays which will hold VBO and IBO data
        //------------------------------------------------
        
        GLfloat* vertices = new GLfloat[PATCH_WIDTH * PATCH_HEIGHT * 2];
        GLuint* indices = new GLuint[PATCH_WIDTH * (2*PATCH_WIDTH+1)];

        for(int i = 0; i < PATCH_WIDTH; ++i) {
            for(int j = 0; j < PATCH_HEIGHT; ++j) {
                //vertex
                int offset = (i * PATCH_WIDTH + j) * 2;
                
                vertices[offset + 0] = i * (1 / (PATCH_COUNT_X * PATCH_WIDTH)); //x 
                vertices[offset + 1] = j * (1 / (PATCH_COUNT_Y * PATCH_HEIGHT)); //z
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
            
            for(int j = 0; j < PATCH_HEIGHT; ++j) { //l = patch_height
                //odd or even?
                indices[offset + direction*(j*2 + 0)] = ((i + a) * PATCH_HEIGHT + j);
                indices[offset + direction*(j*2 + 1)] = ((i + b) * PATCH_HEIGHT + j);
            }
        }
        
        for(int i = 0; i < (PATCH_COUNT_X * PATCH_COUNT_Y); ++i) {
            Patch thePatch;
            thePatch.lod = 0;
            thePatch.offset[0] = (i % PATCH_COUNT_X) * (1 / PATCH_COUNT_X);
            thePatch.offset[1] = (floorf(i / PATCH_COUNT_Y)) * (1 / PATCH_COUNT_Y);
            patches[i] = thePatch;
        }
        
        //----------------------------------
        // set up VBO and IBO, set variables
        //----------------------------------
        
        indexCount = (2*PATCH_WIDTH + 1) * PATCH_HEIGHT;
        
        //generating and binding the opengl buffer
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * (PATCH_WIDTH + 1) * (PATCH_HEIGHT + 1) * 2, vertices, GL_STATIC_DRAW);
        
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