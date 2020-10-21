#pragma once
#include <Windows.h>
#include <hidsdi.h>
#include <hidpi.h>
#include <vector>
#include <string>


namespace TChapman500
{
	namespace JoystickAPI
	{
		class InputDevice
		{
		public:
			InputDevice(HANDLE device);
			virtual ~InputDevice();
			virtual void ProcessInputs(RAWINPUT *rawInput) = 0;

			std::wstring DeviceName;

			HANDLE Device;
		};
	}
}
