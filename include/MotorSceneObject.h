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
#include "Vector3.h"
#include "matrix.h"
#include <vector>

namespace Motor {
    
    struct AnimState {
        int     startframe;              // first frame
        int     endframe;                // last frame
        int     fps;                     // frame per second for this animation
        int     type;                    // animation type
        int     curr_frame;              // current frame
        int     next_frame;              // next frame
        float   timetracker;
    };

	class Model;

	class SceneObject
	{
	public:
		SceneObject* getParent(){ return parent; }
		SceneObject* attachChild(SceneObject* childNode);
		void detachChild(SceneObject* childNode);

		const Vector3& setPosition(const Vector3& pos){ position = pos; matrixDirty = true; return position; }
		const Vector3& getPosition(){ return position; }

		float setScale(float newScale){ scale = newScale; matrixDirty = true; return scale; }
		float getScale(){ return scale; }

		float getYaw(){ return yaw; }
		float getPitch(){ return pitch; }
		float getRoll(){ return roll; }
		float setYaw(float Yaw){ yaw = Yaw; matrixDirty = true; return yaw; }
		float setPitch(float Pitch){ pitch = Pitch; matrixDirty = true; return pitch; }
		float setRoll(float Roll){ roll = Roll; matrixDirty = true; return roll; }

		bool visible;

		//TODO: reference system? mesh->addRef();
		void setModel(const Model* _model);
		const Model* getModel() const { return model; }
        
        AnimState* getState() const { return state; }
        
        void setAnimation(int anim);
        void update(float timeElapsed);

		//Generates a new matrix if dirty
		const mat& getMoveMatrix();
		const mat getFullMoveMatrix(); //Including parent

	private: //Private constructor/deconstructor so only Scene can create/destroy objects
		friend class Scene;
		SceneObject();
		~SceneObject(void);
        
		//The position and orientation of the parent node will be added
		SceneObject* parent;
		std::vector<SceneObject*> childNodes;

		mat moveMatrix;
		bool matrixDirty;

		Vector3 position;
		float yaw, pitch, roll; //The angles of the object. TODO: save in better form
		float scale;

        struct AnimState* state;

		const Model* model;
		//TODO:
		//pointer to animation state object (contains bone positions and so on)
	};
}
