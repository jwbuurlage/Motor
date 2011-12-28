//Singleton

#pragma once
#include "MotorSingleton.h"

namespace Motor {

	class Logger : public Singleton<Logger>
	{
	public:
		Logger();
		~Logger();

		enum LOGTYPE{ NONE=0, INFO, WARNING, ERROR, CRITICALERROR };

		//TODO
		//support for << operator, with special-end-line so logger knows when error ends
		//so that it can prepend a timestamp inbetween different logs
		//logger << ERROR << "OpenGL failed with error code " << errorCode << " which is pretty bad" << logEnd;
		void log(LOGTYPE type, const char* logText); //log will append newline
	};

}
