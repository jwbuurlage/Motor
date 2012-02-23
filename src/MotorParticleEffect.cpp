#include "MotorParticleEffect.h"

namespace Motor {

	ParticleEffect::ParticleEffect(){
		origin = Vector3(0.0f);
		material = 0;
		quadWidth = quadHeight = 1.0f;
	}

	ParticleEffect::~ParticleEffect(){
	}

	void ParticleEffect::update(float elapsedTime){
		for( std::vector<Particle>::iterator part = particles.begin(); part != particles.end(); ++part ){
			part->position += part->movement * elapsedTime;
			part->scale += part->scaleSpeed * elapsedTime;
		}
		return;
	}
}