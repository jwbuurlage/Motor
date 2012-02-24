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
		float size;
		float timeLeft;
	};

	class ParticleEffect
	{
	public:
		//General properties
		Vector3 origin;
		const Material* material;
		float emitRate; //amount of particles per second
		float initialSpeedMax;
		float initialSpeedMin;
		float initialPitchMin; //Angle with plane
		float initialPitchMax;
		float initialYawMin; //Rotation around Y-axis, with positive z-axis as zero angle
		float initialYawMax;
		float initialSize; //width and height of a quad
		float sizeSpeed; //ParticleSize += sizeSpeed * elapsedTime;
		float lifeTime;
		float fadeInEnd; //particle will fade in from start to fadeInEnd
		float fadeOutStart; //fades out from fadeOutStart till lifeTime

		//Let the demo application seed, so that the seed can be
		//send accross network, so everyone sees the same smoke
		//TODO: the current implementation obviously wrong since srand() affects all rand() calls
		//and not only the rand() calls made by this particle emitter. So we have to write a custom
		//random number generator class that allows seperate seeds to be set.
		void setRandomSeed(unsigned int seed){ randomseed = seed; srand(seed); }

		//Wether it is currently emitting. calling disable will still leave the existing particles and fade them out
		void enable(){ enabled = true; fxTimer = 0.0f; }
		void disable(){ enabled = false; }
		bool isEnabled() const { return enabled; }

		void update(float elapsedTime);
	private: //Private constructor/deconstructor so only Scene can create/destroy objects
		friend class Scene;
		ParticleEffect();
		~ParticleEffect(void);

		bool enabled;

		unsigned int randomseed;

		friend class Renderer;
		std::vector<Particle> particles;

		float fxTimer; //internal variable for getting the right emit rate

		void emitParticle();
	};
}
