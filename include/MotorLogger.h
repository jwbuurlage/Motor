//Singleton

//USAGE:
//Logger::getSingleton() << Logger::WARNING << "Texture not found. File: " << someVariable << ". Try again!" << endLog;
//Do NOT forget the endLog at the end of a log message.
//OR use
//LOG_WARNING("Texture not found. File: " << someVariable << ". Try again!");

#pragma once
#include "MotorSingleton.h"
#include <sstream>

//Note: the do{ }while(false) is so that you can use this as a function with a ; at the end:  LOG_WARNING("Hello");
#define LOG_WARNING(MSG)		do{ Motor::Logger::getSingleton() << Motor::Logger::WARNING		<< MSG << Motor::endLog; }while(false)
#define LOG_ERROR(MSG)			do{ Motor::Logger::getSingleton() << Motor::Logger::ERROR		<< MSG << Motor::endLog; }while(false)
#define LOG_CRITICALERROR(MSG)	do{ Motor::Logger::getSingleton() << Motor::Logger::CRITICALERROR	<< MSG << Motor::endLog; }while(false)
#define LOG_INFO(MSG)			do{ Motor::Logger::getSingleton() << Motor::Logger::INFO		<< MSG << Motor::endLog; }while(false)
#define LOG_DEBUG(MSG)			do{ Motor::Logger::getSingleton() << Motor::Logger::DEBUG		<< MSG << Motor::endLog; }while(false)

namespace Motor {

	class Logger : public Singleton<Logger>
	{
	public:
		Logger();
		~Logger();

		//Log level: 
		enum LOGLEVEL{ //Powers of 2
			NONE=0,
			DEBUG = 1,
			INFO = 2,
			WARNING = 4,
			ERROR = 8,
			CRITICALERROR = 16
		};

		//combine the flags like DEBUG | ERROR | CRITICALERROR
		int consoleLogLevel;
		int fileLogLevel;

		void log(LOGLEVEL type, const char* logText); //log will append newline

		void flush(); //ends the current log message

		//Do NOT use this directly!
		//I wasnt able to make the templated << operator a friend of this class so
		//I had to make this function public
		std::stringstream& getStream(){ return streambuff; }
	private:
		std::stringstream streambuff;
		LOGLEVEL currentLogLevel; //The log type that is currently in the streambuff

		friend Logger& operator<<(Logger& logger, Logger::LOGLEVEL lvl);
	};

	//So you can do logger << endLog;
	inline Logger& endLog(Logger& logger){
		logger.flush();
		return logger;
	};
	inline Logger& operator<<(Logger& logger, Logger& (*manipulatorFunc)(Logger&) ){
		return manipulatorFunc(logger);;
	};

	inline Logger& operator<<(Logger& logger, Logger::LOGLEVEL lvl){
		logger.currentLogLevel = lvl;
		switch(lvl){
		case Logger::CRITICALERROR:
			logger.streambuff.str("Critical ERROR: ");
			break;
		case Logger::ERROR:
			logger.streambuff.str("ERROR: ");
			break;
		case Logger::WARNING:
			logger.streambuff.str("Warning: ");
			break;
		case Logger::INFO:
			logger.streambuff.str("Info: ");
			break;
		case Logger::DEBUG:
			logger.streambuff.str("Debug: ");
			break;
		default:
			break;
		};
		return logger;
	}
	//All << operations that stringstream can do:
	template<class T>
	inline Logger& operator<<(Logger& logger, const T& t){
		logger.getStream() << t;
		return logger;
	}

}
