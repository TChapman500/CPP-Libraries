#include "InputButton.h"

namespace TChapman500
{
	namespace JoystickAPI
	{
		InputButton::InputButton(unsigned short buttonID)
		{
			ButtonID = buttonID;
			Pressed = false;
			RisingEdge = false;
			FallingEdge = false;
		}

		void InputButton::SetValue(unsigned long value)
		{
			bool pressed = value > 0;
			// State Changed
			if (pressed != Pressed)
			{
				Pressed = pressed;

				if (pressed)
				{
					RisingEdge = true;
					FallingEdge = false;
				}
				else
				{
					RisingEdge = false;
					FallingEdge = true;
				}
			}
			else
			{
				RisingEdge = false;
				FallingEdge = false;
			}
		}
		bool InputButton::IsButton() { return true; }
		bool InputButton::IsHAT() { return false; }
	}
}