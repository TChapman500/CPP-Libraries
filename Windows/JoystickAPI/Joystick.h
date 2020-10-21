#pragma once
#include <Windows.h>
#include <hidsdi.h>
#include <hidpi.h>
#include "InputDevice.h"

namespace TChapman500
{
	namespace JoystickAPI
	{
		class InputButton;
		class InputAxis;
		class InputHAT;

		class Joystick : public InputDevice
		{
		public:
			Joystick(HANDLE device);
			virtual ~Joystick();
			virtual void ProcessInputs(RAWINPUT *rawInput) override;

			void AddCustomHAT(unsigned short up, unsigned short down, unsigned short right, unsigned short left, std::wstring name);

			std::vector<InputButton *> ButtonList;
			std::vector<InputAxis *> AxisList;
			std::vector<InputHAT *> HATList;

		private:
			USAGE ButtonPage;
			std::vector<InputAxis *> _ValueList;
			std::vector<InputButton *> _ButtonList;
		};
	}
}
