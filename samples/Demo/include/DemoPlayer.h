/* This document is part of the Motor project,
 * an experimental graphics engine.
 *
 *	Jan-Willem Buurlage, 	j.buurlage@students.uu.nl. 
 *	Tom Bannink, 			t.bannink@students.uu.nl
 *
 * http://www.github.com/jwbuurlage/Motor 
 * 
 */
#pragma once
#include "Motor.h"

namespace Demo {
	
	class Player {
	public:
		Player(){ sceneObj = 0; movement = Vector3(0.0f); }
		~Player(){}
		Vector3 movement;
		Motor::SceneObject* sceneObj;
	};
	
}