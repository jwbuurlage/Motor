//Scene manager, holds objects
#pragma once
#include "MotorRoot.h"
#include <list>

namespace Motor {
	
	class SceneObject;
	class ParticleEffect;
	class Light;
	class Camera;

	typedef std::list<SceneObject*> ObjectContainer;
	typedef ObjectContainer::iterator ObjectIterator;

	typedef std::list<ParticleEffect*> EffectContainer;
	typedef EffectContainer::iterator EffectIterator;

	typedef std::list<Light*> LightContainer;
	typedef LightContainer::iterator LightIterator;

	class Scene : public FrameListener{
	public:
		Scene();
		~Scene();

		//will create the camera
		int initialize();
		void cleanup();
		
		//update all objects
		void onFrame(float elapsedTime);

		//creating objects
		SceneObject* createObject();
		ParticleEffect* createParticleEffect();
		Light* createLight();
		//deleting objects
		void deleteObject(SceneObject* obj);
		void deleteParticleEffect(ParticleEffect* fx);
		void deleteLight(Light* light);

		Camera* getCamera(){ return camera; }

		void unloadAllObjects();

		ObjectContainer* getObjectList(){ return &objects; }
		EffectContainer* getEffectList(){ return &particleEffects; }
		LightContainer* getLightList(){ return &lights; }

	private:
		Camera* camera;

		ObjectContainer objects;
		EffectContainer particleEffects;
		LightContainer lights;
	};

}
