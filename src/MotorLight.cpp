#include "MotorLight.h"

namespace Motor {

	Light::Light(){
		position = Vector3(0.0f);
		type = LIGHT_POINT;
		enabled = true;
		color[0] = color[1] = color[2] = color[3] = 1.0f;
	}
	
	Light::~Light(){
	}

}