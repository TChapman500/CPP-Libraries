#include "InputSystem.h"
#include "Joystick.h"
#include <Windows.h>
#include <hidsdi.h>

namespace TChapman500
{
	namespace JoystickAPI
	{
		InputSystem *InputSystem::System = nullptr;

		InputSystem::InputSystem(HWND hWnd)
		{
			// Get RAAW Input Device List
			{
				RAWINPUTDEVICE rawInputDevice[2];

				// Joystick
				rawInputDevice[0].hwndTarget = hWnd;
				rawInputDevice[0].dwFlags = RIDEV_DEVNOTIFY | RIDEV_INPUTSINK;
				rawInputDevice[0].usUsage = 4;
				rawInputDevice[0].usUsagePage = 1;

				// Gamepad (deliberately not used)
				rawInputDevice[1].hwndTarget = hWnd;
				rawInputDevice[1].dwFlags = RIDEV_DEVNOTIFY | RIDEV_INPUTSINK;
				rawInputDevice[1].usUsage = 5;
				rawInputDevice[1].usUsagePage = 1;

				RegisterRawInputDevices(rawInputDevice, 1, sizeof(RAWINPUTDEVICE));
			}
		}

		InputSystem::~InputSystem()
		{
		}

		void InputSystem::ReplaceJoystick(unsigned long long index, Joystick *customJoystick)
		{
			if (index < JoystickList.size())
			{
				delete JoystickList[index];
				JoystickList[index] = customJoystick;
			}

		}

		void InputSystem::MessageLoop(LPARAM lParam)
		{
			RAWINPUT *rawInput;
			UINT bufferSize;

			// Get the RAW Input Buffer.
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &bufferSize, sizeof(RAWINPUTHEADER));
			rawInput = (RAWINPUT *)new char[bufferSize];
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, rawInput, &bufferSize, sizeof(RAWINPUTHEADER));

			for (int i = 0; i < JoystickList.size(); i++)
			{
				if (JoystickList[i]->Device == rawInput->header.hDevice)
				{
					JoystickList[i]->ProcessInputs(rawInput);
				}
			}

			delete[] rawInput;
		}

		void InputSystem::DeviceAdded(HANDLE device)
		{
			Joystick *joystick = new Joystick(device);
			JoystickList.push_back(joystick);
		}

		void InputSystem::DeviceRemoved(HANDLE device)
		{
			Joystick *currJoystick = nullptr;
			for (int i = 0; i < JoystickList.size(); i++)
			{
				currJoystick = JoystickList[i];
				if (currJoystick->Device == device)
				{
					JoystickList.erase(JoystickList.begin() + i);
					break;
				}
			}
			if (currJoystick) delete currJoystick;
		}

		void InputSystem::InputLoop()
		{
			char *rawBuffer;
			RAWINPUT *rawInput;
			UINT bufferSize;

			// Get the RAW Input Buffer.
			GetRawInputBuffer(nullptr, &bufferSize, sizeof(RAWINPUTHEADER));
			if (bufferSize > 0)
			{
				rawBuffer = new char[bufferSize];
				rawInput = (RAWINPUT *)rawBuffer;
				GetRawInputBuffer(rawInput, &bufferSize, sizeof(RAWINPUTHEADER));

				DWORD test;

				for (int i = 0; i < JoystickList.size(); i++)
				{
					if (JoystickList[i]->Device == rawInput->header.hDevice)
					{
						JoystickList[i]->ProcessInputs(rawInput);
						rawInput = (RAWINPUT *)((char *)rawInput + rawInput->header.dwSize);
						if ((char *)rawInput > rawBuffer + bufferSize) break;
					}
				}

				delete[] rawBuffer;
			}
		}

		void InputSystem::ProcessXInputDevices()
		{
		}
	}
}
