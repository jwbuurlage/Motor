#pragma once

namespace Motor {

	class Texture;

	class Material{
	public:
		//TODO: a material can consist of multiple blended textures???
		const Texture* texture;
		float color[4];
	};

}