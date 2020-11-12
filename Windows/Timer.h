#pragma once
#include <Windows.h>

namespace TChapman500
{
	class Timer
	{
	public:
		static Timer *Master;

		Timer();

		double Time();
		double RunTime();





	private:

		double Frequency = 0.0;
		LARGE_INTEGER Start;
		LARGE_INTEGER Current;
		LARGE_INTEGER Difference;

	};
}
