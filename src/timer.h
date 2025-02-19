#pragma once
#include <Windows.h>

namespace TChapman500
{
	class timer
	{
	public:
		inline timer()
		{
			LARGE_INTEGER frequency;
			QueryPerformanceFrequency(&frequency);
			Frequency = (double)frequency.QuadPart;
			start();
		}
		
		inline void start()
		{
			Difference.QuadPart = 0ULL;
			QueryPerformanceCounter(&Start);
			Current.QuadPart = Start.QuadPart;
		}

		inline double lap()
		{
			QueryPerformanceCounter(&Current);
			Difference.QuadPart = Current.QuadPart - Start.QuadPart;
			Start.QuadPart = Current.QuadPart;
			return (double)Difference.QuadPart / Frequency;
		}
		
		inline double run_time()
		{
			QueryPerformanceCounter(&Current);
			Difference.QuadPart = Current.QuadPart - Start.QuadPart;
			return (double)Difference.QuadPart / Frequency;
		}

	private:

		double Frequency = 0.0;
		LARGE_INTEGER Start;
		LARGE_INTEGER Current;
		LARGE_INTEGER Difference;
	};
}
