//Singleton

#pragma once

namespace Motor {

	class Logger
	{
	public:
		Logger();
		~Logger();

		enum LOGTYPE{ NONE=0, INFO, WARNING, ERROR, CRITICALERROR };

		static Logger& getSingleton();

		//TODO
		//support for << operator, with special-end-line so logger knows when error ends
		//so that it can prepend a timestamp inbetween different logs
		//logger << ERROR << "OpenGL failed with error code " << errorCode << " which is pretty bad" << logEnd;
		void log(LOGTYPE type, char* logText); //log will append newline
	};

}