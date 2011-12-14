#pragma once

namespace Motor {

	class Timer{
	public:
		Timer();
		~Timer();

		void initialize();

		//Time since previous call in seconds
		float getElapsedTime();
	private:
#ifdef _WIN32
		float invFrequency;
		__int64 previousTime;
#endif
#ifdef __APPLE__
		uint64_t start, end;
#endif
#ifdef __LINUX__
		double previousTime;
#endif
	};

}