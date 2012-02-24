#include "MotorParticleEffect.h"
#define _USE_MATH_DEFINES
#include <math.h>

namespace Motor {

	ParticleEffect::ParticleEffect(){
		origin = Vector3(0.0f);
		material = 0;
		emitRate = 0.0f;
		initialSpeedMax = 1.0f;
		initialSpeedMin = 0.0f;
		initialPitchMin = -(float)(0.5*M_PI);
		initialPitchMax = (float)(0.5*M_PI);
		initialYawMin = 0.0f;
		initialYawMax = (float)(2.0*M_PI);
		initialSize = 1.0f;
		sizeSpeed = 1.0f;
		lifeTime = 1.0f;
		fadeInEnd = 0.0f;
		fadeOutStart = 0.0f;
		randomseed = 0;
		fxTimer = 0.0f;
		enabled = false;
		particles.reserve(100);
	}

	ParticleEffect::~ParticleEffect(){
	}

	float getRand(float min, float max){
		return min+((float)rand()/(float)RAND_MAX)*(max-min);
	}

	void ParticleEffect::update(float elapsedTime){
		if( enabled ){
			if( emitRate > 0.0f ){
				fxTimer += elapsedTime;
				int toAdd;
				if( particles.empty() ) toAdd = 1+(int)(emitRate*elapsedTime);
				else{ //Use the time that the last particle emitted has lived
					toAdd = (int)(fxTimer*emitRate);
					//instead of setting fxTimer to zero, leave the part of fxTimer that got rounded down
					//so that it is used for the next loop
					fxTimer -= ((float)toAdd)/emitRate;
				}
				for( int i = 0; i < toAdd; ++i ){
					emitParticle();
				}
			}
		}

		for( std::vector<Particle>::iterator part = particles.begin(); part != particles.end(); ){
			part->timeLeft -= elapsedTime;
			if( part->timeLeft <= 0.0f ){
				part = particles.erase(part);
				continue;
			}
			part->position += part->movement * elapsedTime;
			part->size += sizeSpeed * elapsedTime;

			++part;
		}
		return;
	}

	void ParticleEffect::emitParticle(){
		Particle newParticle;
		newParticle.position = origin;
		newParticle.size = initialSize;
		newParticle.timeLeft = lifeTime;
		float speed = getRand(initialSpeedMin, initialSpeedMax);
		float pitch = getRand(initialPitchMin, initialPitchMax);
		float yaw = getRand(initialYawMin, initialYawMax);
		newParticle.movement = Vector3(0.0f, 0.0f, 1.0f);
		newParticle.movement.rotateX(-pitch);
		newParticle.movement.rotateY(yaw);
		newParticle.movement *= speed;
		particles.push_back(newParticle);
	}
}