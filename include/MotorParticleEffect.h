//ParticleEffect can be any type of effect, like a SmokeCloud or a lightbeam
//a ParticleEffect object may only be created with Scene::createParticleEffect

//ParticleEffect will hold a list of all the particles visible on screen

#pragma once
#include "Vector3.h"
#include <vector>

namespace Motor {

	class Material;

	struct Particle{
		Vector3 position;
		Vector3 movement;
		float scale;
		float scaleSpeed; //scale += scaleSpeed * deltaTime;
	};

	class ParticleEffect
	{
	public:
		//General properties
		Vector3 origin;

		const Material* material;

		void update(float elapsedTime);

		//Current state
		float quadWidth, quadHeight;
		//Position, with 'origin' as zero
		std::vector<Particle> particles;

	private: //Private constructor/deconstructor so only Scene can create/destroy objects
		friend class Scene;
		ParticleEffect();
		~ParticleEffect(void);
	};
}
