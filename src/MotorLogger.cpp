#include "MotorLogger.h"
#include <sstream>
#include <iostream>
#include <fstream>

namespace Motor {

	template<> Logger* Singleton<Logger>::singleton = 0;

	Logger::Logger(void)
	{
		consoleLogLevel = INFO | WARNING | ERROR | CRITICALERROR;
		fileLogLevel = WARNING | ERROR | CRITICALERROR;
		currentLogLevel = NONE;
	}

	Logger::~Logger(void)
	{
	}

	void Logger::log(LOGLEVEL type, const char* logText)
	{
		*this << type << logText << endLog;
	}

	void Logger::flush()
	{
		if( consoleLogLevel & currentLogLevel ){
			std::cout << streambuff.str() << std::endl;
		}
		if( fileLogLevel & currentLogLevel ){
			//Note: When doing file output, also prepend timestamp
		}
		streambuff.str(std::string());
	}
}
