#pragma once
#include "InputHAT.h"

namespace TChapman500
{
	namespace JoystickAPI
	{
		class InputButton;

		class CustomHAT : public InputHAT
		{
		public:
			CustomHAT(InputButton *up, InputButton *down, InputButton *right, InputButton *left);
			void SetValue(unsigned long value) override;

		};
	}
}
