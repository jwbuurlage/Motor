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
		localPlayer = 0;
		mainLights[0] = 0;
		mainLights[1] = 0;
		tempObjects[0] = 0;
		tempObjects[1] = 0;

		draggingLeftMouse = false;
		draggingRightMouse = false;
		goingForward = goingBackward = goingLeft = goingRight = goingUp = goingDown = false;
	}

	Game::~Game(){
		if( motorRoot ) delete motorRoot;
		motorRoot = 0;
		if( localPlayer ) delete localPlayer;
		localPlayer = 0;
	}

	void Game::run(){
		if( motorRoot == 0 ) motorRoot = new Motor::Root;
		motorRoot->initialize();
		motorRoot->addInputListener(this);
		motorRoot->addFrameListener(this);

		if( localPlayer == 0 ) localPlayer = new Player;
		if( localPlayer->sceneObj == 0 ) localPlayer->sceneObj = motorRoot->getScene()->createObject();
		localPlayer->sceneObj->setMesh( Motor::MeshManager::getSingleton().getMesh("default") );

		if( mainLights[0] == 0 ) mainLights[0] = motorRoot->getScene()->createLight();
		if( mainLights[1] == 0 ) mainLights[1] = motorRoot->getScene()->createLight();
		tempLightTimer = 0.0f;

		if( tempObjects[0] == 0 ) tempObjects[0] = motorRoot->getScene()->createObject();
		if( tempObjects[1] == 0 ) tempObjects[1] = motorRoot->getScene()->createObject();
		tempObjects[0]->setMesh( Motor::MeshManager::getSingleton().getMesh("default") );
		tempObjects[0]->scale = 0.2f;
		tempObjects[1]->setMesh( Motor::MeshManager::getSingleton().getMesh("default") );
		tempObjects[1]->scale = 0.2f;

		motorRoot->startRendering();
		motorRoot->cleanup();
		return;
	}

	void Game::onFrame(float elapsedTime){
		if( localPlayer ){
			localPlayer->sceneObj->position += localPlayer->movement * elapsedTime;
		}
		tempLightTimer += elapsedTime;
		if( mainLights[0] ){
			mainLights[0]->position.x = 3.0f*sin(tempLightTimer);
			mainLights[0]->position.z = 2.0f*cos(tempLightTimer);
			if( tempObjects[0] ){
				tempObjects[0]->position = mainLights[0]->position;
			}
		}
		if( mainLights[1] ){
			mainLights[1]->position.x = 1.5f*sin(tempLightTimer*2.0f);
			mainLights[1]->position.y = 4.0f*cos(tempLightTimer*2.0f);
			if( tempObjects[1] ){
				tempObjects[1]->position = mainLights[1]->position;
			}
		}
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
			if( localPlayer ){
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
				localPlayer->movement = moveDir * 10.0f; //10 units per second. should be taken from player->movespeed ?
			}
		}
		return keyHandled;

	}

	bool Game::mouseDown(Motor::MOUSEBUTTON button, bool buttonDown, int x, int y){
		//TODO: Send to UI manager and see if it was handled. If not, then do this:
		if( button == Motor::BUTTON_LEFT ){
			draggingLeftMouse = (buttonDown == true);
		}else if( button == Motor::BUTTON_RIGHT ){
			draggingRightMouse = (buttonDown == true);
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
		if( draggingRightMouse ){
			Motor::Camera* cam = motorRoot->getScene()->getCamera();
			if( cam ){
				cam->rotateCamera( -(float)dx/250.0f, -(float)dy/250.0f );
			}
		}
		return false;
	}

}