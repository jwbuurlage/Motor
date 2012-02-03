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
#if defined __APPLE__ || defined __linux__
		double previousTime;
#endif
	};

}
