#pragma once
#include <Windows.h>

namespace TChapman500
{
	class Timer
	{
	public:
		Timer()
		{
			// Get the PC frequency.
			LARGE_INTEGER frequency;
			QueryPerformanceFrequency(&frequency);
			Frequency = (double)frequency.QuadPart;
			
			// Get the current time.
			QueryPerformanceCounter(&Start);
			Current.QuadPart = Start.QuadPart;
		}
		
		// For backwards-compatibility only.
		// Use Lap() Instead.
		double Time()
		{
			// Get the current time.
			QueryPerformanceCounter(&Current);
			
			// Calculate the amount of time that has passed since the last call.
			Difference.QuadPart = Current.QuadPart - Start.QuadPart;
			
			// Start a new lap.
			Start.QuadPart = Current.QuadPart;
			
			// Calculate the time in seconds, then return.
			return (double)Difference.QuadPart / Frequency;
		}
		
		double Lap()
		{
			// Get the current time.
			QueryPerformanceCounter(&Current);
			
			// Calculate the amount of time that has passed since the last call.
			Difference.QuadPart = Current.QuadPart - Start.QuadPart;
			
			// Start a new lap.
			Start.QuadPart = Current.QuadPart;
			
			// Calculate the time in seconds, then return.
			return (double)Difference.QuadPart / Frequency;
		}
		
		double RunTime()
		{
			// Get the current time.
			QueryPerformanceCounter(&Current);
			
			// Calculate the amount of time that has passed since the last call.
			Difference.QuadPart = Current.QuadPart - Start.QuadPart;
			
			// Calcualte the time in seconds, then return.
			return (double)Difference.QuadPart / Frequency;
		}

	private:
		double Frequency = 0.0;		// Performance counter frequency (ticks per second).
		LARGE_INTEGER Start;		// Tick count where this timer was initialized (reset by Lap()).
		LARGE_INTEGER Current;		// Tick count where Lap() or RunTime() was called.
		LARGE_INTEGER Difference;	// Current - Start
	};
}
