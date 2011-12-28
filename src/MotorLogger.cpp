#include "MotorLogger.h"
#include <sstream>
#include <iostream>
#include <fstream>

namespace Motor {

	template<> Logger* Singleton<Logger>::singleton = 0;

	Logger::Logger(void)
	{
	}

	Logger::~Logger(void)
	{
	}

	void Logger::log(LOGTYPE type, const char* logText)
	{
		std::stringstream output;
		switch( type ){
		case WARNING:
			output << "Warning: "; break;
		case ERROR:
			output << "ERROR: "; break;
		case CRITICALERROR:
			output << "CRITICAL ERROR: "; break;
		case NONE:
		case INFO:
		default:
			break;
		}
		output << logText;
		output << std::endl;
		std::cout << output.str();
	}
}
