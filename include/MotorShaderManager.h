#pragma once
#include "MotorSingleton.h"
#include <string>
#include <vector>
#include <map>
#include <GL/glew.h>

namespace Motor {

	class ShaderManager : public Singleton<ShaderManager> {
	public:
		enum ShaderType{ Vertex, Fragment };

		//ShaderManager::Shader class
		//Represents a single vertex or fragment shader
		//It can be attached to different programs
		class Shader {
		public:
			Shader(ShaderType _type);
			~Shader();
			bool load(const char* filename);
			bool compile();
			GLuint getHandle(){ return handle; }
			void setLinkError(bool err){ linkError = err; }
		private:
			ShaderType type;
			bool loaded;
			bool compiled;
			bool linkError;
			GLuint handle;
		};

		//ShaderManager::ShaderProgram class
		//Represents a program that holds vertex and fragment shaders
		//A program can hold multiple shaders of the same type as long as there is only one main() function.
		class ShaderProgram {
		public:
			ShaderProgram();
			~ShaderProgram();

			void attachShader(Shader* shader);
			bool link();
			void use();
			void enableAttribs();
			GLuint getHandle(){ return handle; }
			bool isLinked(){ return linked; }
			//attribs
			void bindAttrib(GLuint index, const char* attribName);
			void vertexAttribPointer(const char* name, GLint size, GLenum type, bool normalize, GLsizei stride, const GLvoid* data);
			void vertexAttribPointer(GLuint index, GLint size, GLenum type, bool normalize, GLsizei stride, const GLvoid* data);
			//uniforms
			GLuint getUniformLocation(const char* varname);
			void setUniform1f(const char* varname, float value);
			void setUniform1i(const char* varname, int value);
			void setUniform3fv(const char* varname,const float* values);
			void setUniformMatrix4fv(const char* varname,const  float* mat);
		private:
			bool linked;
			GLuint handle;
			std::vector<Shader*> shaders;
			//container with attribs and uniforms
			typedef std::map<std::string, GLuint> AttribContainer;
			typedef AttribContainer::iterator AttribIterator;
			AttribContainer attribs;
		};

		//
		// ShaderManager
		//

		ShaderManager();
		~ShaderManager();
		
		//unloads all programs and shaders
		void unloadAllShaders();
		
		//Making programs with the shaders:
		//use makeShader, createProgram, attachShader if more than one vertex/fragment shader is needed
		//use makeShaderProgram if the program has one vertex and one fragment shader
		//after both of these, do bindAttrib and then linkProgram
		Shader* makeShader(const char* shaderName, ShaderType type, const char* sourceFile); //returns 0 on compile error
		ShaderProgram* createProgram(const char* programName);
		void attachShader(const char* programName, const char* shaderName);
		
		//Returns null pointer when file not found or compile error
		ShaderProgram* makeShaderProgram(const char* programName, const char* vertexShaderFile, const char* fragmentShaderFile);

		void bindAttrib(const char* programName, const char* attribName, GLuint index);
		bool linkProgram(const char* programName);

		//Choosing current shader
		void setActiveProgram(const char* programName);
		ShaderProgram* getActiveProgram(){ return activeProgram; }
	private:
		ShaderProgram* activeProgram;
		typedef std::map<std::string,Shader*> ShaderContainer;
		typedef std::map<std::string,ShaderProgram*> ShaderProgramContainer;
		ShaderContainer shaders;
		ShaderProgramContainer shaderPrograms;
	};

}
