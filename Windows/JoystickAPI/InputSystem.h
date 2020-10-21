#pragma once
#include <Windows.h>
#include <vector>

namespace TChapman500
{
	namespace JoystickAPI
	{
		class InputDevice;
		class Joystick;
		class Gamepad;

		class InputSystem
		{
		public:
			static InputSystem *System;

			std::vector<InputDevice *> ControllerList;
			std::vector<Joystick *> JoystickList;
			std::vector<Gamepad *> GamepadList;

			InputSystem(HWND hWnd);
			~InputSystem();

			void ReplaceJoystick(unsigned long long index, Joystick *customJoystick);

			void MessageLoop(LPARAM lParam);
			void DeviceAdded(HANDLE device);
			void DeviceRemoved(HANDLE device);
			void InputLoop();
			void ProcessXInputDevices();
		};
	}
}
