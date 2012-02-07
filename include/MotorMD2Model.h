//
//  MotorMD2Model.h
//  Motor
//
//  Created by Jan-Willem Buurlage on 07-02-12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

namespace Motor {
    
    typedef GLfloat vec3[3];
    
    typedef struct
    {
        short    s;
        short    t;
        
    } texCoo;
    
    typedef struct {
        unsigned char   v[3];                // compressed vertex (x, y, z) coordinates
        unsigned char   lightnormalindex;    // index to a normal vector for the lighting
    } vertex;
    
    typedef struct {
        float scale[3];         // scale values
        float translate[3];     // translation vector
        char name[16];          // frame name
        vertex verts[1];        // first vertex of this frame
    } frame;
    
    typedef struct {
        //FILE INFO:
        int identity;       //this should be equal to "IDP2" if it's an md2 file
        int version;        //this should be equal to 8, this is the md2 version
        
        //MODEL INFO:
        int textureWidth;
        int textureHeight;
        int frameSize;      //size in bytes of each frame in an animation
        
        //NUMBER OF:
        int nTex;           //textures
        int nVertices;      //vertices
        int nTexCoo;        //texture coordinates
        int nTriangles;     //triangles
        int nGLCommands;    //OpenGL commands (GL_TRIANGLE_STRIP or FAN)
        int nFrames;        //frames (you have x frames per animation)
        
        //OFFSETS:
        int oTexNames;      //texture names
        int oTexCoo;        //texture coordinates
        int oTriangles;     //triangles
        int oFrames;        //frames
        int oGLCommands;    //OpenGL commands
        int oEOF;           //end-of-file
        
    } ModelHeader;

    typedef struct
    {
        unsigned short   vert[3];    // indexes to triangle's vertices
        unsigned short   tex[3];     // indexes to vertices' texture coorinates
        
    } triangle;

    
    class MD2Model : public Model {
    public:
        MD2Model() { mesh = 0; material = 0; animated = false; frameCount = 0; frameSize = 0; triangleCount = 0; textureHandle = 0; timetracker = 0.0f; };
        ~MD2Model();
        
        //VARIABLES
        int frameCount;
        int frameSize;
        
        int triangleCount;
        int textureHandle;
        
        static vec3 anorms[162];
        
    private:
                
        
        float timetracker;
        
    };
}