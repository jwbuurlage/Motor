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
#include <vector>
#include <SFML/Network.hpp>

namespace Demo {

	class Minion
	{
		public:
			Motor::SceneObject* sceneObj;
	};

	class AIPlayer
	{
		public:
			AIPlayer(){ socket = 0; }
			~AIPlayer(){ if(socket) delete socket; }

			sf::TcpSocket* socket;
			int UID;

			//Hero
			Motor::SceneObject* heroObj;

			//Minions
			std::vector<Minion> minions;
	};

	class AIGame : public Motor::InputListener, public Motor::FrameListener {
		public:
			AIGame();
			~AIGame();

			void run(); 

			void onFrame(float elapsedTime);

			bool keyDown(sf::Keyboard::Key key, bool keyDown);
			bool mouseDown(Motor::MOUSEBUTTON button, bool buttonDown, int x, int y);
			bool mouseWheelMoved(int delta);
			bool mouseMoved(int x, int y, int dx, int dy);

		private:
			Motor::Root* motorRoot;

			//
			//AI
			//
			std::vector<AIPlayer> players;

			//Handles the full turn based AI system
			//Updates minion posistions and so on
			void updateAI(float elapsedTime);

			//
			//Network
			//
			bool setupServer();
			void networkUpdate(float elapsedTime); //called every frame
			void handlePacket(AIPlayer* player, const sf::Packet& pak);

			//When any network error occurs
			//this variable gets set to false
			//so that network activity is not continued
			bool networkEnabled;
			sf::TcpListener serverSocket;
			sf::TcpSocket* newSocket;
			int newUserIdentifier;
			//float timeUntilRetry;
			//float timeUntilPosUpdate;
			//void handlePacket(sf::Packet* pak);


			//
			//Spectator
			//

			Motor::SceneObject* spectatorNode;
			Vector3 spectatorMovement;
			Vector3 forceDirection; //You excert a force with the keys
			//mouse state
			bool draggingLeftMouse;
			bool draggingRightMouse;
			//For key movement
			bool goingForward, goingBackward, goingLeft, goingRight, goingUp, goingDown, rotatingLeft, rotatingRight;
			bool usingTurbo;

			//
			//Environment
			//
			Motor::SceneObject* groundPlane;
			Motor::Light* mainLights[3];
	};

}
