#pragma once
#include "MotorColladaModel.h"
#include <vector>
#include <string>

namespace Motor{

	class ColladaLoader{
	public:
		ColladaLoader(){ model = 0; };
		~ColladaLoader(){};

		//Filename is for logging purposes
		ColladaModel* loadModel(const char* filename, const char* fileData){ return 0; }

	private:
		ColladaModel* model; //The model we are writing to

		//Recursive
		void processJoint(class domNode* joint, const class mat& parentMatrix, short parentJointIndex);

		void processAnimation(struct SkeletonJoint* targetJoint, class domSampler* sampler, int elementIndex);
	};

}