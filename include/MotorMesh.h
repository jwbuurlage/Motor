//Represents a mesh: vertex data

//Multiple object will have the same mesh
//The current animation state of the mesh will be saved seperately
//This Mesh object only holds the static vertex data

#pragma once
#include <GL/glew.h>

namespace Motor {

	class Material;

	class Mesh{
	public:
		Mesh(){
			hasColor = false;
			hasNormal = false;
			vertexBuffer = 0;
			vertexCount = 0;
			vertexBufferSize = 0;
			primitiveType = 0;
			vertexBufferDataType = GL_FLOAT;
			dimension = 0;
			stride = 0;
			vertexOffset = 0;
			hasIndexBuffer = false;
			indexBuffer = 0;
			indexBufferSize = 0;
			indexBufferDataType = 0;
			indexCount = 0;
			material = 0;
		}
		~Mesh(){
			//TODO: IMPORTANT
			if( vertexBuffer ){
				//delete opengl object
			}
			if( indexBuffer ){
				//delete opengl object
			}
		};

		//Vertex buffer related variables
		bool hasColor;
		bool hasNormal;
		GLuint vertexBuffer;
		GLsizei vertexCount;
		GLsizeiptr vertexBufferSize;
		GLenum primitiveType;
		GLenum vertexBufferDataType; //this should always be GL_FLOAT, so this might be unnecessary 
		GLint dimension; 
		GLsizei stride;
		GLvoid* vertexOffset; //this is needed in case there is no VBO and we want to point to an array in RAM memory
    
		//Indexbuffer related variables
		bool hasIndexBuffer;
		GLuint indexBuffer;
		GLsizeiptr indexBufferSize;
		GLenum indexBufferDataType; //this should always be GL_UNSIGNED_INT, so this might be unnecessary
		GLsizei indexCount;

		//material pointer should be zero if mesh has no material
		const Material* material;
	};

}


#if 0


class Texture;

class Mesh
{
public:

};

#endif
