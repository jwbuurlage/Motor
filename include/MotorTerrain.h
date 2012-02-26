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
    
    enum {
        SEAM_NONE,
        SEAM_TOP,
        SEAM_RIGHT,
        SEAM_BOTTOM,
        SEAM_LEFT,
        SEAM_TOP_RIGHT,
        SEAM_RIGHT_BOTTOM,
        SEAM_BOTTOM_LEFT,
        SEAM_LEFT_TOP
    };
    
    typedef struct {
        Vector3 position;
        float offset[2];
        int lod;
    } Patch;
    
    class TerrainManager {
        
    };
    
    class Terrain {
    public:
        Terrain(Texture* _heightMap, Texture* _normalMap, Texture* _textureMap) { 
            heightMap = _heightMap; 
            normalMap = _normalMap;
            textureMap = _textureMap;
        }
        ~Terrain() { 
            delete[] indexBuffer;
            delete[] indexCount;

        }
        
        void generate(float w_terrain, float l_terrain, float h_terrain);
        void generateIndices();
        
        float heightAtPosition(int x, int y);
        void updatePatches(Vector3 cameraPosition);
        void draw(ShaderManager* shaderManager);
        
        Patch* patches;
        
        GLuint vertexBuffer;
        GLuint* indexBuffer;
        GLuint* indexCount;
        
        mat scaleMatrix;
        
        Texture* heightMap;
        Texture* normalMap;
        Texture* textureMap;
        
        int patchCount() const { return patch_count; };
        int patchSize() const { return patch_count; };

    private:
        int patch_count;
        int patch_size;
    

    };
    
}