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
#include <iostream>

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
        
        Motor::Model* woodBoxModel =  Motor::ModelManager::getSingleton().getModel("Jeep");
        localPlayer->sceneObj->scale = 0.01f;
		localPlayer->sceneObj->setModel(woodBoxModel);
        
        Motor::SceneObject* plane = motorRoot->getScene()->createObject();
        plane->setModel( Motor::ModelManager::getSingleton().getModel("plane") );
        plane->position = Vector3( 0.0f, -3.0f, 0.0f );
        
		Motor::Model* cubeModel = Motor::ModelManager::getSingleton().createModelCopy("sphere", "derpcube");
		Motor::Material* derpMat = Motor::MaterialManager::getSingleton().getMaterial("textures/wood.tga");
		cubeModel->setMaterial( derpMat );
        
        Motor::SceneObject* md2model = motorRoot->getScene()->createObject();
        md2model->scale = 0.01f;
        md2model->setModel( Motor::ModelManager::getSingleton().getModel("Dolphin"));
        md2model->position = Vector3( 5.0f, -0.7f, -5.0f );
        
        Motor::SceneObject* hepModel = motorRoot->getScene()->createObject();
        hepModel->scale = 0.1f;
        hepModel->setModel( Motor::ModelManager::getSingleton().getModel("Hep"));
        hepModel->position = Vector3( 5.0f, -0.7f, 5.0f );

        Motor::SceneObject* archvileModel = motorRoot->getScene()->createObject();
        archvileModel->scale = 0.1f;
        archvileModel->setModel( Motor::ModelManager::getSingleton().getModel("Imp"));
        archvileModel->position = Vector3( -5.0f, -0.7f, 5.0f );

        Motor::SceneObject* bosscubeModel = motorRoot->getScene()->createObject();
        bosscubeModel->scale = 0.2f;
        bosscubeModel->setModel( Motor::ModelManager::getSingleton().getModel("Bosscube"));
        bosscubeModel->position = Vector3( -10.0f, -0.7f, 5.0f );
        
        Motor::SceneObject* cyberModel = motorRoot->getScene()->createObject();
        cyberModel->scale = 0.05f;
        cyberModel->setModel( Motor::ModelManager::getSingleton().getModel("Cyber"));
        cyberModel->position = Vector3( 10.0f, -0.7f, 5.0f );

        Motor::SceneObject* lostSoulModel = motorRoot->getScene()->createObject();
        lostSoulModel->scale = 0.1f;
        lostSoulModel->setModel( Motor::ModelManager::getSingleton().getModel("Lost_Soul"));
        lostSoulModel->position = Vector3( 5.0f, -0.7f, -10.0f ); 

        Motor::SceneObject* md2model2 = motorRoot->getScene()->createObject();
        md2model2->scale = 0.1f;
        md2model2->setModel(Motor::ModelManager::getSingleton().getModel("Ogros"));
        md2model2->position = Vector3( -5.0f, -0.7f, -5.0f );


		for( int i = 0; i < ballCount; ++i ){
			if( balls[i] == 0 ) balls[i] = new Player;
			if( balls[i]->sceneObj == 0 ) balls[i]->sceneObj = motorRoot->getScene()->createObject();
			balls[i]->sceneObj->setModel( cubeModel );
			balls[i]->sceneObj->position = Vector3( 2.0f*sin((float)i), 2.0f*cos((float)i), (float)i );
			balls[i]->sceneObj->scale = 0.2f;
			balls[i]->movement = Vector3( 0, -1.0f, 0);
		}

		for( int i = 0; i < fxCount; ++i ){
			if( effects[i] == 0 ) effects[i] = motorRoot->getScene()->createParticleEffect();
			effects[i]->width = 0.1f;
			effects[i]->height = 0.1f;
			effects[i]->position = Vector3( 0.2f, 0.1f, 0 );
			effects[i]->material = Motor::MaterialManager::getSingleton().getMaterial("textures/derp.png");
		}

		if( mainLights[0] == 0 ) mainLights[0] = motorRoot->getScene()->createLight();
		//if( mainLights[1] == 0 ) mainLights[1] = motorRoot->getScene()->createLight();
		tempLightTimer = 0.0f;

		if( tempObjects[0] == 0 ) tempObjects[0] = motorRoot->getScene()->createObject();
		if( tempObjects[1] == 0 ) tempObjects[1] = motorRoot->getScene()->createObject();
		tempObjects[0]->setModel( Motor::ModelManager::getSingleton().getModel("default") );
		tempObjects[0]->scale = 0.2f;
		tempObjects[1]->setModel( Motor::ModelManager::getSingleton().getModel("default") );
		tempObjects[1]->scale = 0.2f;

		connected = false;
		timeUntilRetry = 0.0f;
		timeUntilPosUpdate = 0.0f;
		socket.SetBlocking(false);

		motorRoot->startRendering();
		motorRoot->cleanup();
		return;
	}

	void Game::onFrame(float elapsedTime){
		doNetworkStuff(elapsedTime);

		if( localPlayer ){
			localPlayer->sceneObj->position += localPlayer->movement * elapsedTime;
		}
		tempLightTimer += elapsedTime;
		if( mainLights[0] ){
			mainLights[0]->position.x = 6.0f*sin(tempLightTimer);
            mainLights[0]->position.y = 14.0f + 8.0f*cos(tempLightTimer);
			mainLights[0]->position.z = 6.0f*cos(tempLightTimer);
			if( tempObjects[0] ){
				tempObjects[0]->position = mainLights[0]->position;
			}
		}
		if( mainLights[1] ){
			mainLights[1]->position.x = 5.5f*sin(tempLightTimer);
			mainLights[1]->position.z = 4.0f*cos(tempLightTimer);
			if( tempObjects[1] ){
				tempObjects[1]->position = mainLights[1]->position;
			}
		}
		for( int i = 0; i < ballCount; ++i ){
			if( balls[i] == 0 ) continue;
			if( balls[i]->sceneObj == 0 ) continue;

			balls[i]->sceneObj->position += balls[i]->movement * elapsedTime;

			const Vector3 gravity(0.0f, -8.0f, 0.0f);
			balls[i]->movement += gravity * elapsedTime;

			if( balls[i]->sceneObj->position.y <= -2.8f  ){
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

//PACKET COMMAND CODES
const int YOURCLIENTID		= 999;
const int PLAYERLIST		= 1000;
const int NEWPLAYER			= 1001;
const int PLAYERDISCONNECT	= 1002;
const int POSITIONUPDATE	= 1003;

	void Game::doNetworkStuff(float elapsedTime){
		if( !connected ){
			timeUntilRetry -= elapsedTime;
			if( timeUntilRetry <= 0.0f ){
				if( socket.Connect(1337, "192.168.1.40") == sf::Socket::Done ){
					connected = true;
					std::cout << "Instantly connected!\n";
				}
				timeUntilRetry = 8.0f;
			}else{
				sf::SelectorTCP selector;
				selector.Add(socket);
				if( selector.Wait(0.001f) ){
					//To check if the socket is connected, we try to read from it:
					sf::Packet pak;
					sf::Socket::Status status = socket.Receive(pak);
					if( status == sf::Socket::Done ){
						connected = true;
						handlePacket(&pak);
					}else if( status == sf::Socket::NotReady ){ //connected but not ready to read
						connected = true;
					}
				}
			}
		}else{ //connected
			sf::Packet packet;

			timeUntilPosUpdate -= elapsedTime;
			if( timeUntilPosUpdate <= 0.0f ){
				if( localPlayer && localPlayer->sceneObj ){
					packet << POSITIONUPDATE << myClientID;
					packet << localPlayer->sceneObj->position.x;
					packet << localPlayer->sceneObj->position.y;
					packet << localPlayer->sceneObj->position.z;
					socket.Send(packet);
				}
				timeUntilPosUpdate = 0.020f;
			}

			sf::Socket::Status status = socket.Receive(packet);
			if( status == sf::Socket::Done ){
				handlePacket(&packet);
			}else if( status != sf::Socket::NotReady ){
				socket.Close();
				connected = false;
				timeUntilRetry = 2.0f;
				std::cout << "Disconnected from server.\n";
			}
		}
	}

	void Game::handlePacket(sf::Packet* pak){
		//Received something
		unsigned int packetCmd;
		*pak >> packetCmd;
		switch( packetCmd ){
		case YOURCLIENTID:
			*pak >> myClientID;
			break;
		case PLAYERLIST:
			{
				//clear old list:
				for( unsigned int i = 0; i < remotePlayers.size(); ++i ){
					if( remotePlayers[i].sceneObj )
						motorRoot->getScene()->deleteObject(remotePlayers[i].sceneObj);
				}
				remotePlayers.clear();

				unsigned int playerCount;
				*pak >> playerCount;
				for( unsigned int i = 0; i < playerCount; ++i ){
					int clientID;
					*pak >> clientID;
					if( clientID != myClientID ){
						Player newPlayer;
						newPlayer.sceneObj = motorRoot->getScene()->createObject();
						newPlayer.sceneObj->setModel( Motor::ModelManager::getSingleton().getModel("default") );
						newPlayer.clientID = clientID;
						remotePlayers.push_back(newPlayer);
					}
				}
			}
			break;
		case NEWPLAYER:
			{
				int clientID;
				*pak >> clientID;
				if( clientID != myClientID ){
					Player newPlayer;
					newPlayer.sceneObj = motorRoot->getScene()->createObject();
					newPlayer.sceneObj->setModel( Motor::ModelManager::getSingleton().getModel("default") );
					newPlayer.clientID = clientID;
					remotePlayers.push_back(newPlayer);
				}
			}
			break;
		case PLAYERDISCONNECT:
			{
				int clientID;
				*pak >> clientID;
				for( std::vector<Player>::iterator iter = remotePlayers.begin(); iter != remotePlayers.end(); ++iter ){
					if( iter->clientID == clientID ){
						if( iter->sceneObj ) motorRoot->getScene()->deleteObject(iter->sceneObj);
						remotePlayers.erase(iter);
						break;
					}
				}
			}
			break;
		case POSITIONUPDATE:
			{
				unsigned int clientID;
				float x,y,z;
				*pak >> clientID >> x >> y >> z;
				for( unsigned int i = 0; i < remotePlayers.size(); ++i ){
					if( remotePlayers[i].clientID == clientID ){
						remotePlayers[i].sceneObj->position = Vector3(x,y,z);
						break;
					}
				}
			}
			break;
		default:
			break;
		};
	}

}