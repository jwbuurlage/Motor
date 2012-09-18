/* This document is part of the Motor project,
 * an experimental graphics engine.
 *
 *	Jan-Willem Buurlage, 	j.buurlage@students.uu.nl. 
 *	Tom Bannink, 			t.bannink@students.uu.nl
 *
 * http://www.github.com/jwbuurlage/Motor 
 * 
 */
#include "AIGame.hpp"
#include <iostream>

using namespace std;

namespace Demo
{
	AIGame::AIGame()
	{
		motorRoot = 0;

		newSocket = 0;
		newUserIdentifier = 100;

		spectatorNode = 0;
		spectatorMovement = Vector3(0.0f, 0.0f, 0.0f);
		forceDirection = Vector3(0.0f, 0.0f, 0.0f);

		draggingLeftMouse = false;
		draggingRightMouse = false;
		goingForward = goingBackward = goingLeft = goingRight = goingUp = goingDown = false;
		rotatingLeft = rotatingRight = false;
		usingTurbo = false;

		groundPlane = 0;
		mainLights[0] = 0;
		mainLights[1] = 0;
		mainLights[3] = 0;
	}

	AIGame::~AIGame()
	{
		//Deleting Motor::Root will clean up all allocated resources
		if( motorRoot ) delete motorRoot;
		motorRoot = 0;
		if( newSocket ) delete newSocket;
	}

	bool AIGame::setupServer()
	{
		if( serverSocket.listen(1337) != sf::Socket::Done )
		{
			cout << "Unable to listen on port 1337!\n";
			networkEnabled = false;
		}
		else
		{
			cout << "Server started on port 1337 TCP\n";
			serverSocket.setBlocking(false);
			networkEnabled = true;
		}
		return networkEnabled;
	}

	void AIGame::run()
	{
		if( setupServer() == false ) return;

		if( motorRoot == 0 ) motorRoot = new Motor::Root;
		if( motorRoot->initialize() == 0 )
		{
			delete motorRoot;
			motorRoot = 0;
			return;
		}
		motorRoot->addInputListener(this);
		motorRoot->addFrameListener(this);

		motorRoot->getScene()->unloadAllObjects();

		if( spectatorNode == 0 )
		{
			spectatorNode = motorRoot->getScene()->createObject();
			spectatorNode->setPosition(Vector3(0.0f, 0.0f, 0.0f));
		}

		if( groundPlane == 0 )
		{
			groundPlane = motorRoot->getScene()->createObject();
			Motor::Model* planeCopy = Motor::ModelManager::getSingleton().createModelCopy("plane", "planecopy");
			Motor::Material* baanMat = Motor::MaterialManager::getSingleton().getMaterial("textures/stone.tga");
			planeCopy->setMaterial(baanMat);
			groundPlane->setModel(planeCopy);
			groundPlane->setScale(3.0f);
			groundPlane->setPosition( Vector3(0.0f,-3.0f,0.0f) );
		}

		Motor::SceneObject* md2model2 = motorRoot->getScene()->createObject();
		md2model2->setScale(0.1f);
		md2model2->setModel(Motor::ModelManager::getSingleton().getModel("Ogros"));
		md2model2->setPosition( Vector3( -5.0f, -0.7f, -5.0f ) );

		if( mainLights[0] == 0 ) mainLights[0] = motorRoot->getScene()->createLight();
		if( mainLights[1] == 0 ) mainLights[1] = motorRoot->getScene()->createLight();

		motorRoot->startRendering();
		motorRoot->cleanup();
		return;
	}

	void AIGame::onFrame(float elapsedTime)
	{
		networkUpdate(elapsedTime);
		updateAI(elapsedTime);

		if( spectatorNode )
		{
			Motor::Camera* cam = motorRoot->getScene()->getCamera();
			const Vector3& specPos = spectatorNode->getPosition();

			//For normal speeds, friction force proportional to the speed
			if( spectatorMovement.squaredLength() > 1.0f ){
				Vector3 frictionVec( - 3.0f * spectatorMovement );
				spectatorMovement += frictionVec * elapsedTime;
			}else{ //For low speeds, constant friction force
				Vector3 frictionVec( - 3.0f * spectatorMovement.normalisedCopy() );
				spectatorMovement += frictionVec * elapsedTime;
			}

			if( cam != 0 )
			{
				cam->setTargetLocation(specPos, false);

				if( rotatingLeft && !rotatingRight )
					cam->rotateCamera( 1.2f * elapsedTime , 0.0f );
				else if( rotatingRight && !rotatingLeft )
					cam->rotateCamera( -1.2f * elapsedTime , 0.0f );

				spectatorNode->setYaw( cam->getYaw() );
				Vector3 force(forceDirection);
				force.rotateY(cam->getYaw());
				spectatorMovement += force * (usingTurbo ? 150.0f : 70.0f) * elapsedTime;
			}
			spectatorNode->setPosition( specPos + spectatorMovement * elapsedTime );
		}

		static float tempLightTimer = 0.0f;
		tempLightTimer += elapsedTime;
		if( mainLights[0] )
		{
			mainLights[0]->position.x = 20.0f*sin(2.0f*tempLightTimer);
			mainLights[0]->position.y = 9.0f + 2.0f*cos(0.5f*tempLightTimer);
			mainLights[0]->position.z = 20.0f*cos(2.0f*tempLightTimer);
		}
		if( mainLights[1] )
		{
			mainLights[1]->position.x = 5.5f*sin(tempLightTimer);
			mainLights[1]->position.z = 4.0f*cos(tempLightTimer);
		}
		//for( unsigned int i = 0; i < remotePlayers.size(); ++i ){
		//	remotePlayers[i].sceneObj->setPosition( remotePlayers[i].sceneObj->getPosition() + remotePlayers[i].movement * elapsedTime );
		//}

		return;
	}

	bool AIGame::keyDown(sf::Keyboard::Key key, bool keyDown){
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
			case sf::Keyboard::Escape: motorRoot->stopRendering(); break;
			default: keyHandled = false; break;
		}

		if( DirectionChanged ){
			forceDirection.x = forceDirection.y = forceDirection.z = 0.0f;
			if( goingForward ) forceDirection.z -= 1.0f;
			if( goingBackward ) forceDirection.z += 1.0f;
			if( goingLeft ) forceDirection.x -= 1.0f;
			if( goingRight ) forceDirection.x  = 1.0f;
			if( goingUp ) forceDirection.y  = 1.0f;
			if( goingDown ) forceDirection.y -= 1.0f;
			forceDirection.normalise();
		}
		return keyHandled;

	}

	bool AIGame::mouseWheelMoved(int delta)
	{
		Motor::Camera* cam = motorRoot->getScene()->getCamera();
		if( !cam ) return false;
		if( delta > 0 )
		{
			cam->camZoomSpeed -= 5.0f;
		}
		else
		{
			cam->camZoomSpeed += 5.0f;
		}
		return false;
	}

	bool AIGame::mouseDown(Motor::MOUSEBUTTON button, bool buttonDown, int x, int y)
	{
		if( button == Motor::BUTTON_LEFT ){
			draggingLeftMouse = (buttonDown == true);
		}else if( button == Motor::BUTTON_RIGHT ){
			draggingRightMouse = (buttonDown == true);
		}
		return false;
	}

	bool AIGame::mouseMoved(int x, int y, int dx, int dy)
	{
		//if( mDraggingLeftMouse ){
		//	Object* object = mObjectMgr->GetCurrentlyControlledObject();
		//	if( object ){
		//		object->yaw -= (float)dx / 300.0f;
		//		object->pitch -= (float)dy / 300.0f;
		//	}
		//}
		if( draggingRightMouse )
		{
			Motor::Camera* cam = motorRoot->getScene()->getCamera();
			if( cam ) cam->rotateCamera( -(float)dx/250.0f, -(float)dy/250.0f );
		}
		return false;
	}

	//
	//		Network protocol and packet codes
	//
	// When a new client connects the server sends
	//	NEWPLAYER <UID>
	//	PLAYERLIST <playercount> <UID> <UID> ...
	//
	// The new client saves his own UID obtained from the NEWPLAYER packet.
	// The NEWPLAYER packet is also send to all other clients.
	//
	// When a client disconnects the server sends to all clients
	//	PLAYERDISCONNECT <UID>
	//
	// Every turn the server sends to all clients
	//	GAMESTATE <turn> <full game state>
	//
	// Clients can, once per turn, do a single move and send
	//	MOVELIST <Move Type>
	//
	//

	//PACKET COMMAND CODES
	const int PLAYERLIST		= 1000;
	const int NEWPLAYER			= 1001;
	const int PLAYERDISCONNECT	= 1002;
	const int GAMESTATE			= 1003;
	const int MOVELIST			= 1004;

	void AIGame::networkUpdate(float elapsedTime)
	{
		if( networkEnabled == false ) return;

		//Possible socket status values:
		// Done - NotReady - Disconnected - Error
		//
		//Check for new connection
		//We always need to have a new socket ready
		//to accept any new connection.
		//If a new client connects we save the socket
		//in his AIPlayer class and create a new socket
		//to hold in newSocket
		if( newSocket == 0 ) newSocket = new sf::TcpSocket;
		sf::Socket::Status stat = serverSocket.accept(*newSocket);
		if( stat == sf::Socket::Disconnected || stat == sf::Socket::Error )
		{
			cout << "ERROR: Server socket error\n";
			networkEnabled = false;
			return;
		}
		if( stat == sf::Socket::Done )
		{
			//New client

			newSocket->setBlocking(false);

			players.push_back(AIPlayer());
			AIPlayer& player = players.back();
			player.socket = newSocket;
			player.UID = newUserIdentifier++;

			cout << "INFO: Client connected from " << player.socket->getRemoteAddress() << " UID=" << player.UID << endl;

			//Send all players a notification
			//For the new player this is also the
			//way of getting his UID
			sf::Packet newPlayerPak;
			newPlayerPak << NEWPLAYER << player.UID;
			for(unsigned int i = 0; i < players.size(); ++i)
			{
				players[i].socket->send(newPlayerPak);
			}

			//Send the player the list of players
			unsigned int playerCount = players.size();
			sf::Packet playerPak;
			playerPak << PLAYERLIST << playerCount;
			for(unsigned int i = 0; i < playerCount; ++i )
			{
				playerPak << players[i].UID;
			}
			player.socket->send(playerPak);

			//Set up the visuals for the new player
			player.heroObj = motorRoot->getScene()->createObject();
			Motor::SceneObject* jeepObject = motorRoot->getScene()->createChildObject(player.heroObj);
			jeepObject->setModel( Motor::ModelManager::getSingleton().getModel("Jeep") );
			jeepObject->setScale(0.01f);
			jeepObject->setYaw(1.5708f);
		}

		//Check if any client has sent something
		sf::Packet packet;
		for(unsigned int pl = 0; pl < players.size(); ++pl)
		{
			sf::Socket::Status stat = players[pl].socket->receive(packet);
			if( stat == sf::Socket::Disconnected || stat == sf::Socket::Error )
			{
				cout << "INFO: Player " << players[pl].UID << " disconnected.\n";
				//Erasing the player from the vector
				//will delete the socket in the AIPlayer deconstructor
				players.erase(players.begin() + pl);
				pl--; //so the index is fixed next loop
			}
			if( stat == sf::Socket::Done )
			{
				//New data received
				handlePacket(&(players[pl]), packet);
			}
		}

		//Receiving is done. Now check if we need to send something

		//... nope
	}

	void AIGame::handlePacket(AIPlayer* player, const sf::Packet& pak)
	{
		return;
	}
#if 0
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
#endif

	void AIGame::updateAI(float elapsedTime)
	{
		return;
	}

}

int main()
{
	Demo::AIGame game;
	game.run();
	return 0;
}
