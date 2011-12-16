#include "MotorTimer.h"

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif
#ifdef __APPLE__
	#include <mach/mach_time.h>
	#include <time.h>
	#include <stdio.h>
#endif
#ifdef __linux__
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

#ifdef __APPLE__
	void mach_absolute_difference(uint64_t end, uint64_t start, struct timespec *tp) {  
		uint64_t difference = end - start;  
		static mach_timebase_info_data_t info = {0,0};  
    	
		if (info.denom == 0)  
			mach_timebase_info(&info);  
    
		uint64_t elapsednano = difference * (info.numer / info.denom);  
    
		tp->tv_sec = elapsednano * 1e-9;  
		tp->tv_nsec = elapsednano - (tp->tv_sec * 1e9);  
	}  

	void Timer::initialize(){
		getElapsedTime(); //To set initial start time
	}

	float Timer::getElapsedTime(){
		struct timespec tp;
		end = mach_absolute_time();
		mach_absolute_difference(end, start, &tp);
		start = mach_absolute_time();
		return tp.tv_nsec * 1e-9;
	}
#endif

#ifdef __linux__
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
