#include "MotorLogger.h"
#include <iostream>
#include <fstream>

Motor::Logger loggerSingleton;

namespace Motor {

	Logger::Logger(void)
	{
	}


	Logger::~Logger(void)
	{
	}

	Logger& Logger::getSingleton()
	{
		return loggerSingleton;
	}

	void Logger::log(LOGTYPE type, char* logText)
	{
		std::cout << logText << std::endl;
	}
}