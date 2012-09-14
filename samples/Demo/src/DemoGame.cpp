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
		rotatingLeft = rotatingRight = false;
		usingTurbo = false;
		forceDirection.x = forceDirection.y = forceDirection.z = 0.0f;

		remotePlayers.clear();
	}

	Game::~Game(){
		if( motorRoot ) delete motorRoot;
		motorRoot = 0;
		if( localPlayer ) delete localPlayer;
		localPlayer = 0;
	}

	void Game::run(){
		if( motorRoot == 0 ) motorRoot = new Motor::Root;
		if( motorRoot->initialize() == 0 ){
			delete motorRoot;
			motorRoot = 0;
			return;
		}
		motorRoot->addInputListener(this);
		motorRoot->addFrameListener(this);

		motorRoot->getScene()->unloadAllObjects();

		if( localPlayer == 0 ) localPlayer = new Player;
		localPlayer->sceneObj = motorRoot->getScene()->createObject();
		Motor::SceneObject* jeepObject = motorRoot->getScene()->createChildObject(localPlayer->sceneObj);
		jeepObject->setModel( Motor::ModelManager::getSingleton().getModel("Jeep") );
		jeepObject->setScale(0.01f);
		jeepObject->setYaw(1.5708f);
		//localPlayer->sceneObj->setPosition( Vector3(0.0f,-3.0f,0.0f) );
		//localPlayer->sceneObj->setModel(Motor::ModelManager::getSingleton().getModel("models/tiefighter.dae"));
		//localPlayer->sceneObj->setScale(0.08f);


		//        Motor::SceneObject* plane = motorRoot->getScene()->createObject();
		//        Motor::Model* planeCopy = Motor::ModelManager::getSingleton().createModelCopy("plane", "planecopy");
		//        Motor::Material* baanMat = Motor::MaterialManager::getSingleton().getMaterial("textures/stone.tga");
		//        planeCopy->setMaterial(baanMat);
		//        plane->setModel(planeCopy);
		//        plane->setScale(3.0f);
		//        plane->setPosition( Vector3(0.0f,-3.0f,0.0f) );

		Motor::Model* cubeModel = Motor::ModelManager::getSingleton().createModelCopy("sphere", "derpcube");
		Motor::Material* derpMat = Motor::MaterialManager::getSingleton().getMaterial("textures/wood.tga");
		cubeModel->setMaterial( derpMat );

		Motor::SceneObject* colladaModel = motorRoot->getScene()->createObject();
		colladaModel->setModel(Motor::ModelManager::getSingleton().getModel("models/cow.dae"));
		colladaModel->setScale(2.0f);
		colladaModel->setPosition( Vector3( -20.0f, 5.0f, -20.0f ) );

		colladaModel = motorRoot->getScene()->createObject();
		colladaModel->setModel(Motor::ModelManager::getSingleton().getModel("models/astroBoy_walk.dae"));
		colladaModel->setPosition( Vector3( -20.0f, 5.0f, 0.0f ) );
		colladaModel->setPitch(-(float)M_PI*0.5f);

		colladaModel = motorRoot->getScene()->createObject();
		colladaModel->setModel(Motor::ModelManager::getSingleton().getModel("models/duck_triangulate.dae"));
		colladaModel->setScale(0.02f);
		colladaModel->setPosition( Vector3( -20.0f, 5.0f, 20.0f ) );

		colladaModel = motorRoot->getScene()->createObject();
		colladaModel->setModel(Motor::ModelManager::getSingleton().getModel("models/guitar.dae"));
		colladaModel->setScale(0.01f);
		colladaModel->setPosition( Vector3( -20.0f, 5.0f, 30.0f ) );

		colladaModel = motorRoot->getScene()->createObject();
		colladaModel->setModel(Motor::ModelManager::getSingleton().getModel("models/modelo.dae"));
		colladaModel->setScale(0.01f);
		colladaModel->setPosition( Vector3( -20.0f, 5.0f, 40.0f ) );

		colladaModel = motorRoot->getScene()->createObject();
		colladaModel->setModel(Motor::ModelManager::getSingleton().getModel("models/neuro.dae"));
		colladaModel->setScale(0.01f);
		colladaModel->setPosition( Vector3( -20.0f, 5.0f, 50.0f ) );

		colladaModel = motorRoot->getScene()->createObject();
		colladaModel->setModel(Motor::ModelManager::getSingleton().getModel("models/tiefighter.dae"));
		colladaModel->setScale(0.08f);
		colladaModel->setPosition( Vector3( -20.0f, 5.0f, 60.0f ) );

		Motor::SceneObject* hepModel = motorRoot->getScene()->createObject();
		hepModel->setScale(0.1f);
		hepModel->setModel( Motor::ModelManager::getSingleton().getModel("Hep"));
		hepModel->setPosition( Vector3( 5.0f, -0.7f, 5.0f ) );

		Motor::SceneObject* archvileModel = motorRoot->getScene()->createObject();
		archvileModel->setScale(0.1f);
		archvileModel->setModel( Motor::ModelManager::getSingleton().getModel("Imp"));
		archvileModel->setPosition( Vector3( -5.0f, -0.7f, 5.0f ) );

		Motor::SceneObject* bosscubeModel = motorRoot->getScene()->createObject();
		bosscubeModel->setScale(0.2f);
		bosscubeModel->setModel( Motor::ModelManager::getSingleton().getModel("Bosscube"));
		bosscubeModel->setPosition( Vector3( -10.0f, -0.7f, 5.0f ) );

		Motor::SceneObject* cyberModel = motorRoot->getScene()->createObject();
		cyberModel->setScale(0.05f);
		cyberModel->setModel( Motor::ModelManager::getSingleton().getModel("Cyber"));
		cyberModel->setPosition( Vector3( 10.0f, -0.7f, 5.0f ) );

		Motor::SceneObject* lostSoulModel = motorRoot->getScene()->createObject();
		lostSoulModel->setScale(0.2f);
		lostSoulModel->setModel( Motor::ModelManager::getSingleton().getModel("Lost_Soul"));
		lostSoulModel->setPosition( Vector3( 0.0f, 6.0f, -25.0f ) );
		lostSoulModel->setYaw((float)M_PI); //rotated 180 degrees

		Motor::SceneObject* md2model2 = motorRoot->getScene()->createObject();
		md2model2->setScale(0.1f);
		md2model2->setModel(Motor::ModelManager::getSingleton().getModel("Ogros"));
		md2model2->setPosition( Vector3( -5.0f, -0.7f, -5.0f ) );


		for( int i = 0; i < ballCount; ++i ){
			if( balls[i] == 0 ) balls[i] = new Player;
			if( balls[i]->sceneObj == 0 ) balls[i]->sceneObj = motorRoot->getScene()->createObject();
			balls[i]->sceneObj->setModel( cubeModel );
			balls[i]->sceneObj->setPosition( Vector3( 2.0f*sin((float)i), 2.0f*cos((float)i), (float)i ) );
			balls[i]->sceneObj->setScale(0.4f);
			balls[i]->movement = Vector3( 0, -1.0f, 0);
		}

		for( int i = 0; i < fxCount; ++i ){
			if( effects[i] == 0 ) effects[i] = motorRoot->getScene()->createParticleEffect();
			effects[i]->origin = Vector3( 2.0f, 1.0f, 3.0f );
			effects[i]->emitRate = 0.0f;
			effects[i]->material = Motor::MaterialManager::getSingleton().getMaterial("textures/smoke_particle.png");
			effects[i]->initialSpeedMin = 2.0f;
			effects[i]->initialSpeedMax = 4.0f;
			effects[i]->initialPitchMin = 0.5f; //little above plane
			effects[i]->initialSize = 0.5f;
			effects[i]->sizeSpeed = 1.8f;
			effects[i]->lifeTime = 5.0f;
			effects[i]->fadeInEnd = 1.0f;
			effects[i]->fadeOutStart = 3.0f;
		}

		if( mainLights[0] == 0 ) mainLights[0] = motorRoot->getScene()->createLight();
		//if( mainLights[1] == 0 ) mainLights[1] = motorRoot->getScene()->createLight();
		tempLightTimer = 0.0f;

		if( tempObjects[0] == 0 ) tempObjects[0] = motorRoot->getScene()->createObject();
		if( tempObjects[1] == 0 ) tempObjects[1] = motorRoot->getScene()->createObject();
		tempObjects[0]->setModel( Motor::ModelManager::getSingleton().getModel("sphere") );
		tempObjects[0]->setScale(0.2f);
		tempObjects[1]->setModel( Motor::ModelManager::getSingleton().getModel("sphere") );
		tempObjects[1]->setScale(0.2f);

		connected = false;
		timeUntilRetry = 0.0f;
		timeUntilPosUpdate = 0.0f;
		socket.setBlocking(false);

		motorRoot->startRendering();
		motorRoot->cleanup();
		return;
	}

	void Game::onFrame(float elapsedTime){
		doNetworkStuff(elapsedTime);

		if( localPlayer ){
			Motor::Camera* cam = motorRoot->getScene()->getCamera();
			const Vector3& playerPos = localPlayer->sceneObj->getPosition();

			if( effects[0] ){
				effects[0]->origin = playerPos;
				effects[0]->emitRate = (usingTurbo ? 50.0f : 5.0f);
			}

			//For normal speeds, friction force proportional to the speed
			if( localPlayer->movement.squaredLength() > 1.0f ){
				Vector3 frictionVec( - 3.0f * localPlayer->movement );
				localPlayer->movement += frictionVec * elapsedTime;
			}else{ //For low speeds, constant friction force
				Vector3 frictionVec( - 3.0f * localPlayer->movement.normalisedCopy() );
				localPlayer->movement += frictionVec * elapsedTime;
			}

			if( cam ){
				cam->setTargetLocation(playerPos, false);
				if( rotatingLeft && !rotatingRight ){
					cam->rotateCamera( 1.2f * elapsedTime , 0.0f );
				}else if( rotatingRight && !rotatingLeft ){
					cam->rotateCamera( -1.2f * elapsedTime , 0.0f );
				}
				localPlayer->sceneObj->setYaw( cam->getYaw() );
				Vector3 force(forceDirection);
				force.rotateY(cam->getYaw());
				localPlayer->movement += force * (usingTurbo ? 150.0f : 70.0f) * elapsedTime;
			}
			localPlayer->sceneObj->setPosition( playerPos + localPlayer->movement * elapsedTime );
		}
		tempLightTimer += elapsedTime;
		if( mainLights[0] ){
			mainLights[0]->position.x = 20.0f*sin(2.0f*tempLightTimer);
			mainLights[0]->position.y = 9.0f + 2.0f*cos(0.5f*tempLightTimer);
			mainLights[0]->position.z = 20.0f*cos(2.0f*tempLightTimer);
			if( tempObjects[0] ){
				tempObjects[0]->setPosition( mainLights[0]->position );
			}
		}
		if( mainLights[1] ){
			mainLights[1]->position.x = 5.5f*sin(tempLightTimer);
			mainLights[1]->position.z = 4.0f*cos(tempLightTimer);
			if( tempObjects[1] ){
				tempObjects[1]->setPosition( mainLights[1]->position );
			}
		}
		for( int i = 0; i < ballCount; ++i ){
			if( balls[i] == 0 ) continue;
			if( balls[i]->sceneObj == 0 ) continue;

			const Vector3 gravity(0.0f, -20.0f, 0.0f);
			balls[i]->movement += gravity * elapsedTime;

			const Vector3& pos = balls[i]->sceneObj->getPosition();
			if( pos.y <= -2.8f  ){
				if( balls[i]->movement.y < 0 ) balls[i]->movement.y = - balls[i]->movement.y;
			}
			balls[i]->sceneObj->setPosition( pos + balls[i]->movement * elapsedTime );
		}

		for( unsigned int i = 0; i < remotePlayers.size(); ++i ){
			remotePlayers[i].sceneObj->setPosition( remotePlayers[i].sceneObj->getPosition() + remotePlayers[i].movement * elapsedTime );
		}

		return;
	}

	bool Game::keyDown(sf::Keyboard::Key key, bool keyDown){
		bool keyHandled = true;

		bool DirectionChanged = false;

		switch( key ){
			case sf::Keyboard::W: goingForward = keyDown;	DirectionChanged = true; break;
			case sf::Keyboard::S: goingBackward = keyDown;	DirectionChanged = true; break;
			case sf::Keyboard::A: rotatingLeft = keyDown;	break;
			case sf::Keyboard::D: rotatingRight = keyDown;	break;
			case sf::Keyboard::Q: goingLeft = keyDown;		DirectionChanged = true; break;
			case sf::Keyboard::E: goingRight = keyDown;		DirectionChanged = true; break;
			case sf::Keyboard::Z: goingDown = keyDown;		DirectionChanged = true; break;
			case sf::Keyboard::X: goingUp = keyDown;		DirectionChanged = true; break;
			case sf::Keyboard::F: usingTurbo = keyDown;		break;
			case sf::Keyboard::Escape:
			case sf::Keyboard::T: motorRoot->stopRendering(); break;
			case sf::Keyboard::C:
								  if( keyDown && effects[0] ){
									  if( effects[0]->isEnabled() ) effects[0]->disable();
									  else effects[0]->enable();
								  }
								  break;
			default:
								  keyHandled = false;
								  break;
		}

		if( DirectionChanged ){
			if( localPlayer ){
				forceDirection.x = forceDirection.y = forceDirection.z = 0.0f;
				if( goingForward ) forceDirection.z -= 1.0f;
				if( goingBackward ) forceDirection.z += 1.0f;
				if( goingLeft ) forceDirection.x -= 1.0f;
				if( goingRight ) forceDirection.x  = 1.0f;
				if( goingUp ) forceDirection.y  = 1.0f;
				if( goingDown ) forceDirection.y -= 1.0f;
				forceDirection.normalise();
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
	const int MOVEMENTUPDATE	= 1003;

	void Game::doNetworkStuff(float elapsedTime){
		if( !connected ){
			timeUntilRetry -= elapsedTime;
			if( timeUntilRetry <= 0.0f ){
				if( socket.connect("127.0.0.1", 1337) == sf::Socket::Done ){
					connected = true;
					std::cout << "Instantly connected!\n";
				}
				timeUntilRetry = 8.0f;
			}else{
				sf::SocketSelector selector;
				selector.add(socket);
				if( selector.wait(sf::seconds(0.001f)) ){
					//To check if the socket is connected, we try to read from it:
					sf::Packet pak;
					sf::Socket::Status status = socket.receive(pak);
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
					const Vector3& pos = localPlayer->sceneObj->getPosition();
					packet << MOVEMENTUPDATE << myClientID;
					packet << pos.x << pos.y << pos.z;
					packet << localPlayer->sceneObj->getPitch();
					packet << localPlayer->sceneObj->getYaw();
					packet << localPlayer->sceneObj->getRoll();
					packet << localPlayer->movement.x;
					packet << localPlayer->movement.y;
					packet << localPlayer->movement.z;
					socket.send(packet);
				}
				timeUntilPosUpdate = 0.050f;
			}

			sf::Socket::Status status = socket.receive(packet);
			if( status == sf::Socket::Done ){
				handlePacket(&packet);
			}else if( status != sf::Socket::NotReady ){
				socket.disconnect();
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
							newPlayer.clientID = clientID;

							Motor::SceneObject* jeepObject = motorRoot->getScene()->createChildObject(newPlayer.sceneObj);
							jeepObject->setModel( Motor::ModelManager::getSingleton().getModel("Jeep") );
							jeepObject->setScale(0.01f);
							jeepObject->setYaw(1.5708f);

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
						newPlayer.clientID = clientID;

						Motor::SceneObject* jeepObject = motorRoot->getScene()->createChildObject(newPlayer.sceneObj);
						jeepObject->setModel( Motor::ModelManager::getSingleton().getModel("Jeep") );
						jeepObject->setScale(0.01f);
						jeepObject->setYaw(1.5708f);

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
			case MOVEMENTUPDATE:
				{
					int clientID;
					float x,y,z;
					float pitch, yaw, roll;
					float movX, movY, movZ;
					*pak >> clientID >> x >> y >> z >> pitch >> yaw >> roll >> movX >> movY >> movZ;
					for( unsigned int i = 0; i < remotePlayers.size(); ++i ){
						if( remotePlayers[i].clientID == clientID ){
							remotePlayers[i].sceneObj->setPosition( Vector3(x,y,z) );
							remotePlayers[i].sceneObj->setPitch(pitch);
							remotePlayers[i].sceneObj->setYaw(yaw);
							remotePlayers[i].sceneObj->setRoll(roll);
							remotePlayers[i].movement = Vector3( movX, movY, movZ );
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
