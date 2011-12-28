#pragma once
#include <GL/glew.h>

namespace Motor {

	class Texture{
	public:
		Texture(){ handle = 0; width = 0; height = 0; }
		~Texture(){ if( handle ) glDeleteTextures(1, &handle); }
		GLuint handle;
		GLuint width;
		GLuint height;
	};

}
