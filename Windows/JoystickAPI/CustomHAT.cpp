#include "CustomHAT.h"
#include "InputButton.h"

namespace TChapman500
{
	namespace JoystickAPI
	{
		CustomHAT::CustomHAT(InputButton *up, InputButton *down, InputButton *right, InputButton *left) : InputHAT()
		{
			Up = up;
			Down = down;
			Right = right;
			Left = left;
		}

		void CustomHAT::SetValue(unsigned long value)
		{
			if (Up->Pressed)
			{
				if (Right->Pressed) value = 2;
				else if (Left->Pressed) value = 8;
				else value = 1;
			}
			else if (Right->Pressed)
			{
				if (Down->Pressed) value = 4;
				else value = 3;
			}
			else if (Down->Pressed)
			{
				if (Left->Pressed) value = 6;
				else value = 5;
			}
			else if (Left->Pressed) value = 7;
			else value = 0;

			InputHAT::SetValue(value);
		}
	}
}
