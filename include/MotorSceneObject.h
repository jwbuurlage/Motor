//SceneObject is a visible object in the scene.
//This is any object with a mesh
//The other possible entity type is Motor::ParticleEffect

//Reason that it is called SceneObject and not Object is that the application
//will probably want to use Object for something else.

//The application should keep its own list of players/objects etc
//and each of those should keep a pointer to a Motor::SceneObject
//which represents its renderable part in the scene

//objects may only be created with Scene::createObject

#pragma once
#include "vector3.h"

namespace Motor {

	class Mesh;

	class SceneObject
	{
	public:
		Vector3 position;
		float yaw, pitch, roll; //The angles of the object. TODO: save in better form
		
		bool visible;
		float scale;

		//TODO: reference system? mesh->addRef();
		void setMesh(const Mesh* _mesh){ mesh = _mesh; }
		const Mesh* getMesh() const { return mesh; }

	private: //Private constructor/deconstructor so only Scene can create/destroy objects
		friend class Scene;
		SceneObject();
		~SceneObject(void);

		const Mesh* mesh;
		//TODO:
		//pointer to animation state object (contains bone positions and so on)
	};
}