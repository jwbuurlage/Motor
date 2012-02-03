#include "MotorTimer.h"

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif
#if defined __linux__ || defined __APPLE__
	#include <sys/time.h>
#endif

namespace Motor {

	Timer::Timer(){
	}

	Timer::~Timer(){
	}

#ifdef _WIN32
	void Timer::initialize(){
		__int64 freq;
		QueryPerformanceFrequency( (LARGE_INTEGER*)&freq );
		invFrequency = 1.0f/float(freq);
		QueryPerformanceCounter( (LARGE_INTEGER*)&previousTime );
	}

	float Timer::getElapsedTime(){
		__int64 NewTime;
		QueryPerformanceCounter( (LARGE_INTEGER*)&NewTime );
		float TimeDiff = float(NewTime - previousTime)*invFrequency;
		previousTime = NewTime;
		return TimeDiff;
	}
#endif

#if defined __APPLE__ || defined __linux__
	void Timer::initialize(){
		getElapsedTime(); //To set initial start time
	}

	float Timer::getElapsedTime(){
		timeval timer;
		gettimeofday(&timer, 0);
		double newTime = timer.tv_sec+(timer.tv_usec/1000000.0);
		float timeDiff = newTime - previousTime;
		previousTime = newTime;
		return timeDiff;
	}
#endif

}
