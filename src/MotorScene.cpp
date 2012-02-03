#include "MotorScene.h"
#include "MotorSceneObject.h"
#include "MotorParticleEffect.h"
#include "MotorLight.h"
#include "MotorCamera.h"
#include "MotorLogger.h"

namespace Motor {

	Scene::Scene(){
	}
	Scene::~Scene(){
		cleanup();
	}

	int Scene::initialize(){
		camera = new Camera;
		return 1;
	}

	void Scene::cleanup(){
		unloadAllObjects();
		if( camera ) delete camera;
		camera = 0;
	}

	void Scene::unloadAllObjects(){

	}

	void Scene::onFrame(float elapsedTime){
		//Iterate through SceneObjects, ParticleEffects and Lights
		//for(ObjectIterator iter = objects.begin(); iter != objects.end(); ++iter)
		//for(EffectIterator iter = effects.begin(); iter != effects.end(); ++iter)
		//for(LightIterator iter = lights.begin(); iter != lights.end(); ++iter)
		if( camera ) camera->update(elapsedTime);
	}

	SceneObject* Scene::createObject(){
		SceneObject* obj = new SceneObject;
		objects.push_back(obj);
		return obj;
	}

	ParticleEffect* Scene::createParticleEffect(){
		ParticleEffect* fx = new ParticleEffect;
		particleEffects.push_back(fx);
		return fx;
	}

	Light* Scene::createLight(){
		if( lights.size() > 8 ){
			Logger::getSingleton().log(Logger::WARNING, "Can not create more than 8 lights!");
		}
		Light* light = new Light;
		lights.push_back(light);
		return light;
	}

	void Scene::deleteObject(SceneObject* obj){
		if( obj == 0 ) return;
		for(ObjectIterator iter = objects.begin(); iter != objects.end(); ++iter){
			if( obj == *iter ){
				objects.erase(iter);
				break;
			}
		}
		delete obj;
	}

	void Scene::deleteParticleEffect(ParticleEffect* fx){
		if( fx == 0 ) return;
		for(EffectIterator iter = particleEffects.begin(); iter != particleEffects.end(); ++iter){
			if( fx == *iter ){
				particleEffects.erase(iter);
				break;
			}
		}
		delete fx;
	}

	void Scene::deleteLight(Light* light){
		if( light == 0 ) return;
		for(LightIterator iter = lights.begin(); iter != lights.end(); ++iter){
			if( light == *iter ){
				lights.erase(iter);
				break;
			}
		}
		delete light;
	}

}
