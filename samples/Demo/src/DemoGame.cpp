/* This document is part of the Motor project,
 * an experimental graphics engine.
 *
 *	Jan-Willem Buurlage, 	j.buurlage@students.uu.nl. 
 *	Tom Bannink, 			t.bannink@students.uu.nl
 *
 * http://www.github.com/jwbuurlage/Motor 
 * 
 */
#include "DemoGame.h"

namespace Demo {
	Game::Game(){
		motorRoot = 0;
	}

	Game::~Game(){
		if( motorRoot ) delete motorRoot;
		motorRoot = 0;
	}

	void Game::run(){
		if( motorRoot == 0 ) motorRoot = new Motor::Root;
		motorRoot->initialize();
		motorRoot->addInputListener(this);

		controlledObject = motorRoot->getScene()->createObject();
		controlledObject->mesh = Motor::MeshManager::getSingleton().getMesh("default");

		motorRoot->startRendering();
		motorRoot->cleanup();
		return;
	}

	bool Game::keyDown(int key, bool keyDown){
		bool keyHandled = true;

		bool DirectionChanged = false;

		switch( key ){
		case 'w': goingForward = keyDown;	DirectionChanged = true; break;
		case 's': goingBackward = keyDown;	DirectionChanged = true; break;
		case 'a': goingLeft = keyDown;		DirectionChanged = true; break;
		case 'd': goingRight = keyDown;		DirectionChanged = true; break;
		case 'q': goingUp = keyDown;		DirectionChanged = true; break;
		case 'e': goingDown = keyDown;		DirectionChanged = true; break;
		//case 'r': loadGame(); break;
		//case 'f': useSkill(); break;
		default:
			keyHandled = false;
			break;
		}

		if( DirectionChanged ){
			if( controlledObject ){
				Vector3 moveDir(0.0f);
				if( goingForward ) moveDir.z -= 1.0f;
				if( goingBackward ) moveDir.z += 1.0f;
				if( goingLeft ) moveDir.x -= 1.0f;
				if( goingRight ) moveDir.x  = 1.0f;
				if( goingUp ) moveDir.y  = 1.0f;
				if( goingDown ) moveDir.y -= 1.0f;
				moveDir.normalise();
				Motor::Camera* cam = motorRoot->getScene()->getCamera();
				if( cam ) moveDir.rotateY(cam->getYaw());
				//controlledObject->movement = moveDir * 10.0f; //10 units per second. should be taken from object->movespeed ?
			}
		}
		return keyHandled;

	}

	bool Game::mouseDown(Motor::MOUSEBUTTON button, bool buttonDown, int x, int y){
		//TODO: Send to UI manager and see if it was handled. If not, then do this:
		if( button == Motor::BUTTON_LEFT ){
			mDraggingLeftMouse = (buttonDown == true);
		}else if( button == Motor::BUTTON_RIGHT ){
			mDraggingRightMouse = (buttonDown == true);
		}else if( button == Motor::BUTTON_WHEELUP ){
			Motor::Camera* cam = motorRoot->getScene()->getCamera();
			if( cam ) cam->camZoomSpeed -= 5.0f;
		}else if( button == Motor::BUTTON_WHEELDOWN ){
			Motor::Camera* cam = motorRoot->getScene()->getCamera();
			if( cam ) cam->camZoomSpeed += 5.0f;
		}
		return false;
	}

	bool Game::mouseMoved(int x, int y, int dx, int dy){
		//if( mDraggingLeftMouse ){
		//	Object* object = mObjectMgr->GetCurrentlyControlledObject();
		//	if( object ){
		//		object->yaw -= (float)dx / 300.0f;
		//		object->pitch -= (float)dy / 300.0f;
		//	}
		//}
		if( mDraggingRightMouse ){
			Motor::Camera* cam = motorRoot->getScene()->getCamera();
			if( cam ){
				cam->rotateCamera( -(float)dx/250.0f, -(float)dy/250.0f );
			}
		}
		return false;
	}

}