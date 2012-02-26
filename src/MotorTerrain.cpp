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

#include <math.h>
#include <algorithm>

//windows does not know log2
#ifndef log2
#define log2(x) (log((float)x)/log(2.0f))
#endif

#ifndef max
#define max(a,b) (a>b ? a : b)
#endif

#ifndef min
#define min(a,b) (a<b ? a : b)
#endif


namespace Motor {
 
    void Terrain::generate(float w_terrain, float l_terrain, float h_terrain) {
		if( heightMap == 0 ){
			LOG_ERROR("Heightmap not loaded. Cannot generate terrain.");
			return;
		}
        
        //----------------------
        // make the scale matrix
        //----------------------
        
        scaleMatrix.scaleX(w_terrain).scaleZ(l_terrain).scaleY(h_terrain);
        scaleMatrix.translate(-w_terrain/2, -10.0f, -l_terrain/2);
      
        //-----------------------------------------
        // check if the heightmap is of proper size
        //-----------------------------------------

        int w, h;
        w = heightMap->width;
        h = heightMap->height;
         
        // check if width is of the form $2^n + 1$
        if(!(((w-1) & (w-2)) == 0) || w != h) {
            LOG_WARNING("Height map is of the wrong size. Must be of the form $2^n + 1$, and square.");
            w = 1025;
            h = 1025;
            LOG_INFO("Heightmap: width and height set to default value (1025).");
        }
                
        //-------------------------------------
        // prepare patches, and patch variables
        //-------------------------------------
           
        // calculate patch size, and count -- and generate the VBO
        // we want the patch_size to be of the form $2^n + 1$ as well
        patch_count = 16; //16x16 grid is fine for now.
        patch_size = (1 << (int)(log2(w - 1) - log2(patch_count))) + 1;

        patches = new Patch[patch_count*patch_count];
        
        //------------------------------------------------
        // prepare arrays which will hold VBO and IBO data
        //------------------------------------------------
        
        float* vertices = new float[2*patch_size * patch_size];
        
        // the size of one texel in terms of texture coordinates
        float tex_per_coo = 1 / ((float)((patch_size - 1) * patch_count));
        
        // we populate the VBO
        for(int i = 0; i < patch_size; ++i) {
            for(int j = 0; j < patch_size; ++j) {
                vertices[2*patch_size*i + 2*j] = j * tex_per_coo;
                vertices[2*patch_size*i + 2*j + 1] = i * tex_per_coo;
            }
        }
        
        //generating and binding the opengl buffer
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * patch_size * patch_size * 2, vertices, GL_STATIC_DRAW);
        
        delete[] vertices;
                        
        generateIndices();
        
        for(int i = 0; i < (patch_count * patch_count); ++i) {
            Patch thePatch;
            thePatch.lod = 0;
            thePatch.offset[0] = (i % patch_count) * (1 / (float)patch_count);
            thePatch.offset[1] = (floorf(i / patch_count)) * (1 / (float)patch_count);
            thePatch.position = scaleMatrix * Vector3(thePatch.offset[0], 0.0f, thePatch.offset[1]);
            patches[i] = thePatch;
        }        
    }
    
    void Terrain::generateIndices() {
        // we calculate the amount of levels and make the index buffers
        level_max = log2(patch_size - 1);
        
        indexCount = new GLuint[level_max + 1];
        indexBuffer = new GLuint[level_max + 1];
        
        for(int l = 0; l <= level_max; ++l)
        {
            // make a container for the indices, calculate the
            // patch size for this level, and init the index counter
            int patch_size_level = (1 << (level_max - l)) + 1;
            int current_index = 0;
            indexCount[l] = (2*patch_size_level+1) * patch_size_level;            
            GLuint* indices = new GLuint[indexCount[l]];     
			
			for(int i = 0; i < patch_size - 1; i += (1 << l)){
                if((int)(i/(1 << l)) % 2 == 0) {
                    // dir ---->
                    for(int j = 0; j < patch_size; j += (1 << l)) {
                        indices[current_index] = (i * patch_size) + j;
                        ++current_index;
                        indices[current_index] = ((i + (1 << l)) * patch_size) + j;
                        ++current_index;
                    }
                    indices[current_index] = (i + (1 << l) + 1) * patch_size - 1; //make degenerate triangle
                    ++current_index;
                }
                else {
                    // dir <----
                    for(int j = 0; j < patch_size; j += (1 << l)) {
                        indices[current_index] = (i + 1) * patch_size - 1 - j;
                        ++current_index;
                        indices[current_index] = (i + 1 + (1 << l)) * patch_size - 1 - j;
                        ++current_index;
                    }
                    indices[current_index] = (i + (1 << l)) * patch_size; //make degenerate triangle
                    ++current_index;
                }
            }

            // save the indexbuffer
            glGenBuffers(1, &indexBuffer[l]);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer[l]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indexCount[l], indices, GL_STATIC_DRAW);
          
            delete[] indices;
        }
        
    }
    
    void Terrain::updatePatches(Vector3 cameraPosition) {
        //set distances for now
        for(int i = 0; i < patch_count * patch_count; ++i) {
            Vector3 distanceVector = cameraPosition - patches[i].position;
            float d = distanceVector.length();
            
            if(d < 30.0f) {
                patches[i].lod = 0;
            }
            else if (d < 60.0f) {
                patches[i].lod = min(level_max, 1);
            }
            else if (d < 90.0f) {
                patches[i].lod = min(level_max, 2);
            }
            else  {
                patches[i].lod = min(level_max, 3);
            }        
        }
    }
    
    void Terrain::draw(ShaderManager* shaderManager) {
        //loop through patches and draw them
    }
    
    float Terrain::heightAtPosition(int x, int y) {
        return 0.0f;
    }
}