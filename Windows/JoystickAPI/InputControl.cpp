#include "InputControl.h"

namespace TChapman500
{
	namespace JoystickAPI
	{
		InputControl::InputControl()
		{
		}

		InputControl::~InputControl()
		{
		}

		void InputControl::SetName(std::wstring name)
		{
			ControlName = name;
		}
	}
}
