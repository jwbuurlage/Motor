#include "MotorParticleEffect.h"

namespace Motor {

	ParticleEffect::ParticleEffect(){
		position = Vector3(0.0f);
		material = 0;
		width = 0.1f;
		height = 0.1f;
	}

	ParticleEffect::~ParticleEffect(){
	}

}