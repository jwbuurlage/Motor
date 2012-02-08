#include "MotorSceneObject.h"
#include "MotorModel.h"

namespace Motor {

	SceneObject::SceneObject(){
		position = Vector3(0.0f);
		yaw = pitch = roll = 0.0f;
		visible = true;
		scale = 1.0f;
		model = 0;
        state = 0;
	}

	SceneObject::~SceneObject(){
        if(state) {
            delete state;
        }
	}
    
    void SceneObject::setModel(const Motor::Model *_model) {
        model = _model;
        if(_model->isAnimated()) {
            setAnimation(0);
        }
    }
    
    void SceneObject::setAnimation(int anim) {
        if(!state)
            state = new AnimState;
        
        model->setAnimation(state, anim);
    }

    void SceneObject::update(float timeElapsed) {
        if(state) {
            model->updateAnimationState(state, timeElapsed);
        }
    }
}