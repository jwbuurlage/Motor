#pragma once
#include "MotorSceneObject.h"
#include "MotorModel.h"
#include "matrix.h"
#include <vector>
#include <string>

namespace Motor {

	struct AnimStateJoint{
		int currentFrame;
		int nextFrame;
		float timer;
	};

    class SkeletonAnimState : public AnimStateBase {
	public:
		SkeletonAnimState(){ boneMatrixData = 0; }
		~SkeletonAnimState(){ if( boneMatrixData ) delete[] boneMatrixData; }

		std::vector<AnimStateJoint> jointAnimStates; //index into the joints array is the same as this array
		GLfloat* boneMatrixData; //amount of floats = joints.size() * 16
	};

	struct ColladaSubmesh{
		//Should add different types: triangles, polygons etc
		unsigned int vertexOffset; //byte offset in vertex buffer
		unsigned int indexOffset; //byte offset in index buffer
		unsigned int indexCount;
	};

	struct KeyFrame{
		float time;
		mat bindMatrix;
	};

	struct SkeletonJoint{
		short parentIndex; //parentIndex of root is -1
		short index; //index of root is 0
		mat bindMatrix; //also known as Joint Matrix or local bone transformation matrix
		mat worldMatrix; //parent->worldMatrix * bindMatrix
		mat inverseBindMatrix;
		std::vector<KeyFrame> keyFrames;
		//The following animation is only used during loading. It was a lot easier to just put it in this struct
		//vertex-binding will refer to nodeSID
		//animation target contains a string "nodeID/matrixSid"
		std::string nodeSid; //name of the bone in collada file
		std::string nodeID; //xml identifier in collada file
		std::string matrixSid; //sid of matrix
	};
 
    class ColladaModel : public Model {
    public:
		ColladaModel(){
			modelType = MODELTYPE_COLLADA;
			skeletonEnabled = false;
		};
		~ColladaModel(){
		};

		//In bytes
		inline int getVBOstride() const {return (skeletonEnabled ? 16 : 8)*sizeof(GLfloat);}

		bool skeletonEnabled;

		std::vector<ColladaSubmesh> subMeshes;
		std::vector<SkeletonJoint> joints;

		mat bindShapeMatrix;

		AnimStateBase* createAnimationState() const;
		void updateAnimationState(AnimStateBase* state, float timeElapsed) const;
		void setAnimation(AnimStateBase* state, int _type) const;

		GLfloat* getCurrentMatrixData(AnimStateBase* state);
	private:
		//Recursive function used internally by getCurrentMatrixData
		void getWorldMatrix(int jointIndex, GLfloat* matrixData, std::vector<bool>& finishedList);
    };
}