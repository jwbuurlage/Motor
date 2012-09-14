//Represents a mesh: vertex data

//SceneObjects contains:
// - Model (pointer to Model)
// - Animation state (animation being played, current bone positions)

//Model contains:
// - Mesh (the vertex data)
// - BoneData
// - MaterialList

//Several models can share the same mesh
//A referencecount system is used

#pragma once
#include <GL/glew.h>

namespace Motor {

	class Mesh{
	public:
		Mesh(){
			referenceCount = 0;

			hasColor = false;
			hasNormal = false;
			vertexBuffer = 0;
			vertexCount = 0;
			vertexBufferSize = 0;
			primitiveType = 0;
			dimension = 0;
			stride = 0;
			//vertexOffset = 0;
			hasIndexBuffer = false;
			indexBuffer = 0;
			indexBufferSize = 0;
			indexBufferDataType = 0;
			indexCount = 0;
		}
		~Mesh(){
			if( vertexBuffer ){
				glDeleteBuffers(1, &vertexBuffer);
			}
			if( indexBuffer ){
				glDeleteBuffers(1, &indexBuffer);
			}
		};

		//Vertex buffer related variables
		bool hasColor;
		bool hasNormal;
		GLuint vertexBuffer;
		GLsizei vertexCount;
		GLsizeiptr vertexBufferSize;
		GLenum primitiveType;
		GLint dimension; 
		GLsizei stride;
		//GLvoid* vertexOffset; //this is needed in case there is no VBO and we want to point to an array in RAM memory
    
		//Indexbuffer related variables
		bool hasIndexBuffer;
		GLuint indexBuffer;
		GLsizeiptr indexBufferSize;
		GLenum indexBufferDataType;
		GLsizei indexCount;

		void addRef(){ ++referenceCount; };
		void release(){ --referenceCount; };
	private:
		int referenceCount;
	};

}
