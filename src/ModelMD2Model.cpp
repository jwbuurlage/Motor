//
//  ModelMD2Model.cpp
//  Motor
//
//  Created by Jan-Willem Buurlage on 08-02-12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "MotorModel.h"
#include "MotorMD2Model.h"
#include "MotorSceneObject.h"

//debuf
#include <iostream>

namespace Motor {
    
    animation MD2Model::animationList[21] = 
    {
        // first, last, fps
        {   0,  39,  9 },   // STAND
        {  40,  45, 10 },   // RUN
        {  46,  53, 10 },   // ATTACK
        {  54,  57,  7 },   // PAIN_A
        {  58,  61,  7 },   // PAIN_B
        {  62,  65,  7 },   // PAIN_C
        {  66,  71,  7 },   // JUMP
        {  72,  83,  7 },   // FLIP
        {  84,  94,  7 },   // SALUTE
        {  95, 111, 10 },   // FALLBACK
        { 112, 122,  7 },   // WAVE
        { 123, 134,  6 },   // POINT
        { 135, 153, 10 },   // CROUCH_STAND
        { 154, 159,  7 },   // CROUCH_WALK
        { 160, 168, 10 },   // CROUCH_ATTACK
        { 196, 172,  7 },   // CROUCH_PAIN
        { 173, 177,  5 },   // CROUCH_DEATH
        { 178, 183,  7 },   // DEATH_FALLBACK
        { 184, 189,  7 },   // DEATH_FALLFORWARD
        { 190, 197,  7 },   // DEATH_FALLBACKSLOW
        { 198, 198,  5 },   // BOOM
    };

    
    MD2Model::MD2Model() { 
        mesh = 0; 
        material = 0; 
        animated = false; 
        frameCount = 0; 
        frameSize = 0; 
        triangleCount = 0; 
        textureHandle = 0; 
        timetracker = 0.0f; 
    };
    
    void MD2Model::setAnimation(struct AnimState* state, int _type) const {
		state->type = _type;
		state->fps = animationList[_type].fps;

		//Yes this does happen a lot: models that do not have the animation
		if( animationList[_type].lastFrame < frameCount ){
			//Note: if lastFrame < frameCount then firstFrame < frameCount as well, because these are static values in the table
			state->startframe = animationList[_type].firstFrame;
			state->endframe = animationList[_type].lastFrame;
		}else{
			state->startframe = 0;
			state->endframe = frameCount - 1; //endFrame is 0-based so frameCount-1 is the last frame index
		}

        //reset timetracker
        state->timetracker = 0.0f;

		state->curr_frame = state->startframe;
		state->next_frame = state->startframe + 1;
    }

    void MD2Model::updateAnimationState(AnimState* state, float timeElapsed) const {
        float timeperframe = (float)1/state->fps;
        
        state->timetracker += timeElapsed;
        
        if(state->timetracker > timeperframe) {
            state->curr_frame = state->next_frame;
            state->next_frame = state->curr_frame + 1;
            if(state->next_frame > state->endframe) {
                state->next_frame = state->startframe;
            }
            state->timetracker = 0.0f;
        }
    }
    
}