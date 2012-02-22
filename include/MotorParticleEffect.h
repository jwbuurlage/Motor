//ParticleEffect can be any type of effect, like a SmokeCloud or a lightbeam
//a ParticleEffect object may only be created with Scene::createParticleEffect

//ParticleEffect will hold a list of all the particles visible on screen

#pragma once
#include "Vector3.h"
#include <vector>

namespace Motor {

	class Material;

	class ParticleEffect
	{
	public:
		//General properties
		Vector3 origin;

		const Material* material;

		//Current state
		float quadWidth, quadHeight;
		//Position, with 'origin' as zero
		std::vector<Vector3> particlePositions;

	private: //Private constructor/deconstructor so only Scene can create/destroy objects
		friend class Scene;
		ParticleEffect();
		~ParticleEffect(void);
	};
}
