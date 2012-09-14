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
#include <vector>
#include <SFML/Network.hpp>

namespace Demo {
	
	class Game : public Motor::InputListener, public Motor::FrameListener {
	public:
		Game();
		~Game();
		
		void run(); 
		
		void onFrame(float elapsedTime);

		bool keyDown(int key, bool keyDown);
		bool mouseDown(Motor::MOUSEBUTTON button, bool buttonDown, int x, int y);
		bool mouseWheelMoved(int delta);
		bool mouseMoved(int x, int y, int dx, int dy);

	private:
		Motor::Root* motorRoot;

		sf::TcpSocket socket;
		bool connected;
		float timeUntilRetry;
		float timeUntilPosUpdate;
		void doNetworkStuff(float elapsedTime);
		void handlePacket(sf::Packet* pak);
		
		int myClientID;
		std::vector<Player> remotePlayers;

		//mouse state
		bool draggingLeftMouse;
		bool draggingRightMouse;
		//For key movement
		bool goingForward, goingBackward, goingLeft, goingRight, goingUp, goingDown, rotatingLeft, rotatingRight;
		Vector3 forceDirection;
		bool usingTurbo;

		static const int ballCount = 5;
		Player* balls[ballCount];

		static const int fxCount = 1;
		Motor::ParticleEffect* effects[fxCount];

		Player* localPlayer;
		Motor::SceneObject* tempObjects[2];
		Motor::Light* mainLights[3];
		float tempLightTimer;
	};
	
}
