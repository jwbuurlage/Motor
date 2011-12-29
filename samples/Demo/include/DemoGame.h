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
#include "DemoPlayer.h"

namespace Demo {
	
	class Game : public Motor::InputListener, public Motor::FrameListener {
	public:
		Game();
		~Game();
		
		void run(); 
		
		void onFrame(float elapsedTime);

		bool keyDown(int key, bool keyDown);
		bool mouseDown(Motor::MOUSEBUTTON button, bool buttonDown, int x, int y);
		bool mouseMoved(int x, int y, int dx, int dy);

	private:
		Motor::Root* motorRoot;

		//mouse state
		bool draggingLeftMouse;
		bool draggingRightMouse;
		//For key movement
		bool goingForward, goingBackward, goingLeft, goingRight, goingUp, goingDown;
		Vector3 getMoveDir();

		Player* localPlayer;
		Motor::SceneObject* tempObjects[2];
		Motor::Light* mainLights[2];
		float tempLightTimer;
	};
	
}