#include "MotorSceneObject.h"

namespace Motor {

	SceneObject::SceneObject(){
		position = Vector3(0.0f);
		yaw = pitch = roll = 0.0f;
		visible = true;
		scale = 1.0f;
		mesh = 0;
	}

	SceneObject::~SceneObject(){
	}

}