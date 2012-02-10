#include "MotorShaderManager.h"
#include "MotorFilesystem.h"
#include "MotorLogger.h"

namespace Motor {

	template<> ShaderManager* Singleton<ShaderManager>::singleton = 0;

	//
	// ShaderManager::Shader methods
	//

	ShaderManager::Shader::Shader(ShaderType _type){
		type = _type;
		loaded = false;
		compiled = false;
		linkError = false;
		switch(type){
			case Vertex: { handle = glCreateShader(GL_VERTEX_SHADER); } break;
			case Fragment: { handle = glCreateShader(GL_FRAGMENT_SHADER); } break;
			default: handle = 0; break;
		}
	}

	ShaderManager::Shader::~Shader(){
		//If the shader is still attached, OpenGL will flag it for deletion and delete it as soon as it is detached
		if( handle ) glDeleteShader(handle);
	}

	int ShaderManager::Shader::load(const char* filename){
		pFile shaderFile = Filesystem::getSingleton().getFile(filename);
		if( !shaderFile ){
			Logger::getSingleton().log(Logger::ERROR, "Shader file not found.");
		}else{
			const GLchar* shaderSource = shaderFile->data;
			glShaderSource(handle, 1, &shaderSource, NULL);
			loaded = true;
			compiled = false;
		}
		return loaded;
	}

	int ShaderManager::Shader::compile(){
		if( !loaded ){
			Logger::getSingleton().log(Logger::WARNING, "Trying to compile shader while no source loaded.");
			compiled = false;
		}else{
			glCompileShader(handle);
			GLint value;
			glGetShaderiv(handle, GL_COMPILE_STATUS, &value);
			if( value == GL_TRUE ){
				compiled = true;
			}else{
				compiled = false;
				glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &value);
				GLchar* infoLog = new GLchar[value];
				glGetShaderInfoLog(handle, value, &value, infoLog);
				Logger::getSingleton().log(Logger::ERROR, "Shader compilation error: ");
				Logger::getSingleton().log(Logger::ERROR, infoLog);
				delete[] infoLog;
			}
		}
		return compiled;
	}

	//
	// ShaderManager::ShaderProgram methods
	//

	ShaderManager::ShaderProgram::ShaderProgram(){
		handle = glCreateProgram();
		linked = false;
	}

	ShaderManager::ShaderProgram::~ShaderProgram(){
		for( std::vector<Shader*>::iterator iter = shaders.begin(); iter != shaders.end(); ++iter ){
			glDetachShader(handle, (*iter)->getHandle());
		}
		glDeleteProgram(handle);
	}

	void ShaderManager::ShaderProgram::attachShader(Shader* shader){
		shaders.push_back(shader);
		glAttachShader(handle, shader->getHandle());
	}

	void ShaderManager::ShaderProgram::link(){
		if( linked ){
			Logger::getSingleton().log(Logger::WARNING, "Trying to link shader that is already linked.");
		}else{
			glLinkProgram(handle);

			GLint value;
			glGetProgramiv(handle, GL_LINK_STATUS, &value);
			if( value == GL_TRUE ){
				linked = true;
			}else{
				linked = false;

				//Get the link log info and then the compilation log for all attached shaders.
				//First determine the size of the buffer needed
				GLint maxBufLen = 0;
				glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &value);
				maxBufLen = value;
				for( std::vector<Shader*>::iterator iter = shaders.begin(); iter != shaders.end(); ++iter ){
					glGetShaderiv((*iter)->getHandle(), GL_INFO_LOG_LENGTH, &value);
					if( value > maxBufLen ) maxBufLen = value;
				}
				//The buffer will now be big enough to hold all error messages (not at once!)
				GLchar* infoLog = new GLchar[maxBufLen];

				//Now get the log info
				glGetProgramInfoLog(handle, maxBufLen, NULL, infoLog);

				Logger::getSingleton().log(Logger::ERROR, "Shader link error followed by compilation logs: ");
				Logger::getSingleton().log(Logger::ERROR, infoLog);

				//Display compilation log of all attached shaders
			
				for( std::vector<Shader*>::iterator iter = shaders.begin(); iter != shaders.end(); ++iter ){
					glGetShaderInfoLog((*iter)->getHandle(), value, &value, infoLog);
					Logger::getSingleton().log(Logger::ERROR, infoLog);
					(*iter)->setLinkError(true);
				}

				delete[] infoLog;
			}
		}
	}

	void ShaderManager::ShaderProgram::use(){
		glUseProgram(handle);
	}

	void ShaderManager::ShaderProgram::enableAttribs(){
		for(AttribIterator iter = attribs.begin(); iter != attribs.end(); ++iter ){
			glEnableVertexAttribArray(iter->second);
		}
	}

	void ShaderManager::ShaderProgram::bindAttrib(GLuint index, const char* attribName){
		if(linked){
			Logger::getSingleton().log(Logger::WARNING, "Trying to bind attribute to shader program that is already linked");
		}else{
			attribs.insert(AttribContainer::value_type(attribName, index));
			glBindAttribLocation(handle, index, attribName);
		}
	}

	void ShaderManager::ShaderProgram::vertexAttribPointer(const char* name, GLint size, GLenum type, bool normalize, GLsizei stride, const GLvoid* data){
		const AttribIterator attr = attribs.find(name);
		if( attr != attribs.end() ) {
			glVertexAttribPointer(attr->second, size, type, normalize, stride, data);
		}
	}

	GLuint ShaderManager::ShaderProgram::getUniformLocation(const char* varname){
		return glGetUniformLocation(handle, varname);
	}
	void ShaderManager::ShaderProgram::setUniform1f(const char* varname, float value){
		glUniform1f(getUniformLocation(varname), value);
	}
	void ShaderManager::ShaderProgram::setUniform1i(const char* varname, int value){
		glUniform1i(getUniformLocation(varname), value);
	}
	void ShaderManager::ShaderProgram::setUniform3fv(const char* varname,const  float* values){
		glUniform3fv(getUniformLocation(varname), 1, values);
	}
	void ShaderManager::ShaderProgram::setUniformMatrix4fv(const char* varname, const float* mat){
		glUniformMatrix4fv(getUniformLocation(varname), 1, false, mat);
	}

	//
	// ShaderManager methods
	//
	
	ShaderManager::ShaderManager(){
		activeProgram = 0;
	}

	ShaderManager::~ShaderManager()
	{
		//Unload all shaders
		for( ShaderProgramContainer::iterator iter = shaderPrograms.begin(); iter != shaderPrograms.end(); ++iter ){
			delete iter->second; //will call corresponding opengl functions
		}
		for( ShaderContainer::iterator iter = shaders.begin(); iter != shaders.end(); ++iter ){
			delete iter->second; //will call corresponding opengl functions
		}
	}

	ShaderManager::Shader* ShaderManager::makeShader(const char* shaderName, ShaderType type, const char* sourceFile){
		ShaderContainer::iterator shader = shaders.find(shaderName);
		if( shader != shaders.end() ) return shader->second;
		Shader* newShader = new Shader(type);
		newShader->load(sourceFile);
		newShader->compile();
		shaders.insert(ShaderContainer::value_type(shaderName,newShader));
		return newShader;
	}

	void ShaderManager::createProgram(const char* programName){
		ShaderProgram* newProgram = new ShaderProgram();
		shaderPrograms.insert(ShaderProgramContainer::value_type(programName, newProgram));
	}

	void ShaderManager::attachShader(const char* programName, const char* shaderName){
		ShaderProgramContainer::iterator program = shaderPrograms.find(programName);
		ShaderContainer::iterator shader = shaders.find(shaderName);
		if( program == shaderPrograms.end() ){
			Logger::getSingleton().log(Logger::WARNING, "attachShader could not find program");
		}else if( shader == shaders.end() ){
			Logger::getSingleton().log(Logger::WARNING, "attachShader could not find shader");
		}else{
			program->second->attachShader(shader->second);
		}
	}

	void ShaderManager::makeShaderProgram(const char* programName, const char* vertexShaderFile, const char* fragmentShaderFile){
		//Create the shaders with filename as name
		Shader* vsShader = makeShader(vertexShaderFile, Vertex, vertexShaderFile);
		Shader* fsShader = makeShader(fragmentShaderFile, Fragment, fragmentShaderFile);
		
		ShaderProgram* newProgram = new ShaderProgram();
		shaderPrograms.insert(ShaderProgramContainer::value_type(programName, newProgram));
		newProgram->attachShader(vsShader);
		newProgram->attachShader(fsShader);
	}

	void ShaderManager::bindAttrib(const char* programName, const char* attribName, GLuint index){
		ShaderProgramContainer::iterator program = shaderPrograms.find(programName);
		if( program == shaderPrograms.end() ){
			Logger::getSingleton().log(Logger::WARNING, "bindAttrib could not find program");
		}else{
			program->second->bindAttrib(index, attribName);
		}
	}

	void ShaderManager::linkProgram(const char* programName){
		ShaderProgramContainer::iterator program = shaderPrograms.find(programName);
		if( program == shaderPrograms.end() ){
			Logger::getSingleton().log(Logger::WARNING, "linkProgram could not find program");
		}else{
			program->second->link();
		}
	}

	void ShaderManager::setActiveProgram(const char* programName){
		ShaderProgramContainer::iterator program = shaderPrograms.find(programName);
		if( program == shaderPrograms.end() ){
			Logger::getSingleton().log(Logger::WARNING, "setActiveProgram could not find program");
		}else{
			if( program->second->isLinked() ){
				activeProgram = program->second;
				activeProgram->use();
				activeProgram->enableAttribs();
			}
		}
	}

}
