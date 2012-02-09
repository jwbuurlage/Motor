#include "MotorSceneObject.h"
#include "MotorModel.h"

namespace Motor {

	SceneObject::SceneObject(){
		parent = 0;
		matrixDirty = true;
		position.x = position.y = position.z = 0.0f;
		yaw = pitch = roll = 0.0f;
		visible = true;
		scale = 1.0f;
		model = 0;
        state = 0;
	}

	SceneObject::~SceneObject(){
		//Note: child note deletion is done by Scene
        if(state) {
            delete state;
        }
	}

	SceneObject* SceneObject::attachChild(SceneObject* childNode){
		if( childNode == 0 ) return 0;
		if( childNode == this ) return childNode;
		//Check if it does not already exist
		for( std::vector<SceneObject*>::iterator iter = childNodes.begin(); iter != childNodes.end(); ++iter ){
			if( *iter == childNode ){
				return childNode;
			}
		}
		childNodes.push_back(childNode);
		childNode->parent = this;
		return childNode;
	}
	
	void SceneObject::detachChild(SceneObject* childNode){
		if( childNode == 0 ) return;
		for( std::vector<SceneObject*>::iterator iter = childNodes.begin(); iter != childNodes.end(); ++iter ){
			if( *iter == childNode ){
				childNodes.erase(iter);
				break;
			}
		}
		return;
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

	const mat& SceneObject::getMoveMatrix(){
		if( matrixDirty ){
			moveMatrix.setIdentity();
			moveMatrix.setRotationZ(roll);
			moveMatrix.rotateX(pitch);
			moveMatrix.rotateY(yaw);
			moveMatrix.scale(scale);
			moveMatrix.translate(position);
			matrixDirty = false;
		}
		return moveMatrix;
	}

	const mat SceneObject::getFullMoveMatrix(){
		mat fullMat;
		if( parent ) fullMat *= parent->getFullMoveMatrix();
		fullMat *= getMoveMatrix();
		return fullMat;
	}
}