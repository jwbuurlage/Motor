//
//  MotorMD2Model.h
//  Motor
//
//  Created by Jan-Willem Buurlage on 07-02-12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "MotorSceneObject.h"

namespace Motor {

    class MD2AnimState : public AnimStateBase {
	public:
        int     startframe;              // first frame
        int     endframe;                // last frame
        int     fps;                     // frame per second for this animation
        int     type;                    // animation type
		int		currentFrame;
		int		nextFrame;
		float	timer;
	};

    
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
    

    // We now define an animation list of an md2 model for easier access:
    typedef enum {
        STAND,
        RUN,
        ATTACK,
        PAIN_A,
        PAIN_B,
        PAIN_C,
        JUMP,
        FLIP,
        SALUTE,
        FALLBACK,
        WAVE,
        POINT,
        CROUCH_STAND,
        CROUCH_WALK,
        CROUCH_ATTACK,
        CROUCH_PAIN,
        CROUCH_DEATH, 
        DEATH_FALLBACK,
        DEATH_FALLFORWARD,
        DEATH_FALLBACKSLOW,
        BOOM,
        MAX_ANIMATIONS
    } animationType;

    typedef struct {
        int firstFrame; // first frame of the animation
        int lastFrame;  // number of frames
        int fps;        // number of frames per second
    } animation;

    class MD2Model : public Model {
    public:
        MD2Model();
        ~MD2Model();
        
        //VARIABLES
        int frameCount;
        int frameSize;
        
        int triangleCount;
        int textureHandle;
        
		AnimStateBase* createAnimationState() const { return new MD2AnimState; }
        virtual void updateAnimationState(AnimStateBase* state, float timeElapsed) const;
        virtual void setAnimation(AnimStateBase* state, int _type) const;
        
        virtual int verticesPerFrame() const { return triangleCount * 3; }
        
		float getInterpolation(AnimStateBase* state) const;
		int getVertexOffset(AnimStateBase* state) const;
		int getNextFrameVertexOffset(AnimStateBase* state) const;

        static vec3 anorms[162];
        
    private:
        static animation animationList[21];
        float timetracker;
        
    };
}