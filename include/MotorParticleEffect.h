//ParticleEffect can be any type of effect, like a SmokeCloud or a lightbeam
//a ParticleEffect object may only be created with Scene::createParticleEffect

//ParticleEffect will hold a list of all the particles visible on screen
//However, for now it is only a texture and a position to render that texture with a scale parameter

#pragma once
#include "Vector3.h"

namespace Motor {

	class Material;

	class ParticleEffect
	{
	public:
		Vector3 position; //only x and y are used and are in range [-1,1]
		float width, height; //width 1 means full screen width etc

		const Material* material;

	private: //Private constructor/deconstructor so only Scene can create/destroy objects
		friend class Scene;
		ParticleEffect();
		~ParticleEffect(void);
	};
}
