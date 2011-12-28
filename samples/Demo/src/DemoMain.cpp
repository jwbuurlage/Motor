/* This document is part of the Motor project,
 * an experimental graphics engine.
 *
 *	Jan-Willem Buurlage, 	j.buurlage@students.uu.nl. 
 *	Tom Bannink, 			t.bannink@students.uu.nl
 *
 * http://www.github.com/jwbuurlage/Motor 
 * 
 */

#include "Motor.h"
#include "DemoGame.h"

int main() {
	Motor::Root root;
	Demo::Game game;
	root.initialize();
	Motor::SceneObject* obj = root.getScene()->createObject();
	obj->mesh = Motor::MeshManager::getSingleton().getMesh("default");
	root.startRendering();
	root.cleanup();
	return 0;
}