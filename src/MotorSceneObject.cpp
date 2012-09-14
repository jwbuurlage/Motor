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
        animationState = 0;
	}

	SceneObject::~SceneObject(){
		//Note: child node deletion is done by Scene
        if(animationState) {
            delete animationState;
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
		if( model != _model ){ //Only if new model
			model = _model;
			//We might need a different subclass of AnimStateBase
			if( animationState ) delete animationState;
			animationState = 0;
			if( model && model->isAnimated() ) {
				animationState = model->createAnimationState();
				setAnimation(0);
			}
		}
    }
    
    void SceneObject::setAnimation(int anim) {
		if( model && animationState )
			model->setAnimation(animationState, anim);
    }

    void SceneObject::update(float timeElapsed) {
		if( model && animationState )
			model->updateAnimationState(animationState, timeElapsed);
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
