#include "MotorScene.h"
#include "MotorSceneObject.h"
#include "MotorParticleEffect.h"
#include "MotorLight.h"
#include "MotorCamera.h"
#include "MotorLogger.h"
#include "MotorTextureManager.h"
#include "MotorTerrain.h"

namespace Motor {

	Scene::Scene(){
	}
	Scene::~Scene(){
		cleanup();
	}

	int Scene::initialize(){
		camera = new Camera;
        terrain = new Terrain(TextureManager::getSingleton().getTexture("textures/heightmap.png"), TextureManager::getSingleton().getTexture("textures/normal_map_terrain.bmp"), (Texture*)0);
        terrain->generate(350.0f, 350.0f, 30.0f);
        
        
		return 1;
	}

	void Scene::cleanup(){
		unloadAllObjects();
		if( camera ) delete camera;
		camera = 0;
	}

	void Scene::unloadAllObjects(){
		for(ObjectIterator iter = objects.begin(); iter != objects.end(); ++iter){
			delete *iter;
		}
		objects.clear();

		for(EffectIterator iter = particleEffects.begin(); iter != particleEffects.end(); ++iter){
			delete *iter;
		}
		particleEffects.clear();

		for(LightIterator iter = lights.begin(); iter != lights.end(); ++iter){
			delete *iter;
		}
		lights.clear();
	}

	void Scene::onFrame(float elapsedTime){
		//Iterate through SceneObjects, ParticleEffects and Lights
		for(ObjectIterator iter = objects.begin(); iter != objects.end(); ++iter)
			(*iter)->update(elapsedTime);
		//for(EffectIterator iter = effects.begin(); iter != effects.end(); ++iter)
		//for(LightIterator iter = lights.begin(); iter != lights.end(); ++iter)
		if( camera ) camera->update(elapsedTime);
        if( terrain ) terrain->updatePatches(camera->getPosition());
	}

	SceneObject* Scene::createObject(){
		SceneObject* obj = new SceneObject;
		objects.push_back(obj);
		return obj;
	}

	SceneObject* Scene::createChildObject(SceneObject* parent){
		if( parent ) return parent->attachChild(createObject());	
		return createObject();
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

		if( obj->childNodes.empty() ){

			for(ObjectIterator iter = objects.begin(); iter != objects.end(); ++iter){
				if( obj == *iter ){
					objects.erase(iter);
					break;
				}
			}
			delete obj;

		}else{

			//This should be safe for any nodes hvaing their parents as childs and all that kinds of stuff
			std::vector<SceneObject*> deleteList;
			deleteList.push_back( obj );

			while( !deleteList.empty() ){
				SceneObject* current = deleteList.back();
				deleteList.pop_back();
				//Add its childs to delete list
				for( std::vector<SceneObject*>::iterator iter = current->childNodes.begin(); iter != current->childNodes.end(); ++iter ){
					deleteList.push_back( *iter );
				}
				//Delete this item
				for(ObjectIterator iter = objects.begin(); iter != objects.end(); ++iter){
					if( current == *iter ){
						objects.erase(iter);
						delete current;
						break;
					}
				}
			}

		}
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
