#pragma once
#include "vector3.h"

namespace Motor {

	class Light
	{
	public:
		Vector3 position;
		enum{ LIGHT_POINT, LIGHT_CONE } type;
		bool enabled;
		float color[4];

		//Variables for special type of lights go here

	private: //Private constructor/deconstructor so only Scene can create/destroy lights
		friend class Scene;
		Light();
		~Light(void);
	};
}