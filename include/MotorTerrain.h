//
//  MotorTerrain.h
//  Motor
//
//  Created by Jan-Willem Buurlage on 10-02-12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <GL/glew.h>
#include "Vector3.h"
#include "matrix.h"

namespace Motor {
    class Texture;
    class ShaderManager;
    
    typedef struct {
        float offset[2];
        int lod;
    } Patch;
    
    class TerrainManager {
        
    };
    
    class Terrain {
    public:
        Terrain(Texture* _heightMap, Texture* _normalMap) { 
            heightMap = _heightMap; 
            normalMap = _normalMap;
        }
        ~Terrain() { }
        
        void generate(float w_terrain, float l_terrain, float h_terrain);
        float heightAtPosition(int x, int y);
        void updatePatches();
        void draw(ShaderManager* shaderManager);
        
        Patch* patches;
        
        int PATCH_COUNT_X;
        int PATCH_COUNT_Y;
        int PATCH_WIDTH;
        int PATCH_HEIGHT;
        
        GLuint vertexBuffer;
        GLuint indexBuffer;
        int indexCount;
        int offsets[2];
        
        mat scaleMatrix;
        
        Texture* heightMap;
        Texture* normalMap;
        
    private:

    };
    
}