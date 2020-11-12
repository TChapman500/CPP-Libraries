#include "pch.h"
#include "Timer.h"

namespace TChapman500
{
	Timer *Timer::Master = nullptr;
	Timer::Timer()
	{
		if (!Master)
		{
			LARGE_INTEGER frequency;
			QueryPerformanceFrequency(&frequency);
			Frequency = (double)frequency.QuadPart;
		}
		QueryPerformanceCounter(&Start);
		Current.QuadPart = Start.QuadPart;
		Difference.QuadPart = 0;
	}
	double Timer::Time()
	{
		QueryPerformanceCounter(&Current);
		Difference.QuadPart = Current.QuadPart - Start.QuadPart;
		Start.QuadPart = Current.QuadPart;
		return (double)Difference.QuadPart / Master->Frequency;
	}
	double Timer::RunTime()
	{
		QueryPerformanceCounter(&Current);
		Difference.QuadPart = Current.QuadPart - Start.QuadPart;
		return (double)Difference.QuadPart / Master->Frequency;
	}
}
