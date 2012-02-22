#include "MotorParticleEffect.h"

namespace Motor {

	ParticleEffect::ParticleEffect(){
		origin = Vector3(0.0f);
		material = 0;
		quadWidth = quadHeight = 1.0f;
	}

	ParticleEffect::~ParticleEffect(){
	}

}