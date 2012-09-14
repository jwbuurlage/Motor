#include "MotorColladaModel.h"
#include "MotorSceneObject.h"

namespace Motor{

	AnimStateBase* ColladaModel::createAnimationState() const {
		SkeletonAnimState* animState = new SkeletonAnimState;
		AnimStateJoint newAnimState;
		newAnimState.currentFrame = 0;
		newAnimState.nextFrame = 0;
		newAnimState.timer = 0.0f;
		for(size_t i = 0; i < joints.size(); ++i ){
			animState->jointAnimStates.push_back(newAnimState);
		}
		animState->boneMatrixData = new GLfloat[16*joints.size()];
		return animState;
	}

	void ColladaModel::updateAnimationState(AnimStateBase* _state, float timeElapsed) const {
		SkeletonAnimState* state = (SkeletonAnimState*)_state;

		//Animation states are kept seperately for every joint
		for(size_t i = 0; i < joints.size(); ++i ){
			if( joints[i].keyFrames.empty() ) continue;

			AnimStateJoint& jointState = state->jointAnimStates[i];

			jointState.timer += timeElapsed;

			float frameTime = joints[i].keyFrames[jointState.nextFrame].time - joints[i].keyFrames[jointState.currentFrame].time;
			while( jointState.timer > frameTime ){
				//Go to next frame
				jointState.currentFrame = jointState.nextFrame;
				jointState.nextFrame++;
				if( jointState.nextFrame >= (int)joints[i].keyFrames.size() ){
					jointState.currentFrame = 0;
					jointState.nextFrame = (joints[i].keyFrames.size() > 1 ? 1 : 0);
				}
				jointState.timer -= frameTime;
				frameTime = joints[i].keyFrames[jointState.nextFrame].time - joints[i].keyFrames[jointState.currentFrame].time;
			}
		}
	}

	void ColladaModel::setAnimation(AnimStateBase* _state, int _type) const{
		SkeletonAnimState* state = (SkeletonAnimState*)_state;
		for(size_t i = 0; i < joints.size(); ++i){
			state->jointAnimStates[i].currentFrame = 0;
			if( joints[i].keyFrames.size() > 1 ){
				state->jointAnimStates[i].nextFrame = 1;
			}else{
				state->jointAnimStates[i].nextFrame = 0;
			}
		}
	}

	void ColladaModel::getWorldMatrix(int jointIndex, GLfloat* matrixData, std::vector<bool>& finishedList){
		if( finishedList[jointIndex] == true ) return;

		int parentIndex = joints[jointIndex].parentIndex;
		if( parentIndex == -1 ){
			finishedList[jointIndex] = true;
			return;
		}

		if( finishedList[parentIndex] == false )
			getWorldMatrix(parentIndex, matrixData, finishedList);

		mat& outputMatrix = *reinterpret_cast<mat*>(&matrixData[16*jointIndex]);
		//Apply parent matrix to left side
		outputMatrix = *reinterpret_cast<mat*>(&matrixData[16*parentIndex]) * outputMatrix;
		finishedList[jointIndex] = true;
		return;
	}

	GLfloat* ColladaModel::getCurrentMatrixData(AnimStateBase* _state){
		SkeletonAnimState* state = (SkeletonAnimState*)_state;
		//First we calculate the matrix for each bone, in local bone space
		//Then in the second loop we multiply in a chain from parent to child to obtain
		//the full world space matrix, and finally we apply the inverse bind matrix.
		for(size_t i = 0; i < joints.size(); ++i){
			
			mat& outputMatrix = *reinterpret_cast<mat*>(&(state->boneMatrixData[16*i]));

			AnimStateJoint& jointState = state->jointAnimStates[i];

			if( joints[i].keyFrames.empty() ){
				outputMatrix = joints[i].bindMatrix;
			}else if( jointState.currentFrame == jointState.nextFrame ){
				outputMatrix = joints[i].keyFrames[jointState.currentFrame].bindMatrix;
			}else{
				KeyFrame& curFrame = joints[i].keyFrames[jointState.currentFrame];
				KeyFrame& nextFrame = joints[i].keyFrames[jointState.nextFrame];
				
				float interpolation = jointState.timer / (nextFrame.time - curFrame.time);

				outputMatrix = curFrame.bindMatrix.scaledCopy(1.0f - interpolation);
				outputMatrix += nextFrame.bindMatrix.scaledCopy(interpolation);
			}
		}
		//An array indicated wether we calculated a joints world matrix
		std::vector<bool> matrixFinished(joints.size(), false);
		for(size_t i = 0; i < joints.size(); ++i){
			getWorldMatrix(i, state->boneMatrixData, matrixFinished);
		}
		//Apply inverse-bind-matrix on right side
		for(size_t i = 0; i < joints.size(); ++i){
			*reinterpret_cast<mat*>(&(state->boneMatrixData[16*i])) *= joints[i].inverseBindMatrix;
		}
		return state->boneMatrixData;
	}
}