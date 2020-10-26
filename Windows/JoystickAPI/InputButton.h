#pragma once
#include "InputControl.h"

namespace TChapman500
{
	namespace JoystickAPI
	{
		class InputButton : public InputControl
		{
		public:
			InputButton(unsigned short buttonID);
			void SetValue(unsigned long value) override;
			bool IsButton() override;
			bool IsHAT() override;

			unsigned short ButtonID;
			bool Pressed;
			bool RisingEdge;
			bool FallingEdge;
		};
	}
}
