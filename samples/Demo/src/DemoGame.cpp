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
		for(int i = 0; i < ballCount; ++i){
			balls[i] = 0;
		}
		for(int i = 0; i < fxCount; ++i ){
			effects[i] = 0;
		}

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

		for( int i = 0; i < ballCount; ++i ){
			if( balls[i] == 0 ) balls[i] = new Player;
			if( balls[i]->sceneObj == 0 ) balls[i]->sceneObj = motorRoot->getScene()->createObject();
			balls[i]->sceneObj->setMesh( Motor::MeshManager::getSingleton().getMesh("default") );
			balls[i]->sceneObj->position = Vector3( 2.0f*sin((float)i), 2.0f*cos((float)i), (float)i );
			balls[i]->sceneObj->scale = 0.2f;
			balls[i]->movement = Vector3( 0, -1.0f, 0);
		}

		for( int i = 0; i < fxCount; ++i ){
			if( effects[i] == 0 ) effects[i] = motorRoot->getScene()->createParticleEffect();
			effects[i]->width = 0.1f;
			effects[i]->height = 0.1f;
			effects[i]->position = Vector3( 0.2f, 0.1f, 0 );
		}

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
		for( int i = 0; i < ballCount; ++i ){
			if( balls[i] == 0 ) continue;
			if( balls[i]->sceneObj == 0 ) continue;

			balls[i]->sceneObj->position += balls[i]->movement * elapsedTime;

			const Vector3 gravity(0.0f, -5.0f, 0.0f);
			balls[i]->movement += gravity * elapsedTime;

			if( balls[i]->sceneObj->position.y <= -3.0f  ){
				if( balls[i]->movement.y < 0 ) balls[i]->movement.y = - balls[i]->movement.y;
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
            case 't': motorRoot->stopRendering(); break;  
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

	bool Game::mouseWheelMoved(int delta){
		if( delta > 0 ){
			Motor::Camera* cam = motorRoot->getScene()->getCamera();
			if( cam ) cam->camZoomSpeed -= 5.0f;
		}else{
			Motor::Camera* cam = motorRoot->getScene()->getCamera();
			if( cam ) cam->camZoomSpeed += 5.0f;
		}
		return false;
	}

	bool Game::mouseDown(Motor::MOUSEBUTTON button, bool buttonDown, int x, int y){
		//TODO: Send to UI manager and see if it was handled. If not, then do this:
		if( button == Motor::BUTTON_LEFT ){
			draggingLeftMouse = (buttonDown == true);
		}else if( button == Motor::BUTTON_RIGHT ){
			draggingRightMouse = (buttonDown == true);
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