#pragma once
#include <Windows.h>
#include <hidsdi.h>
#include <hidpi.h>
#include "InputControl.h"

namespace TChapman500
{
	namespace JoystickAPI
	{
		class InputButton;

		class InputHAT : public InputControl
		{
		public:
			InputHAT();
			InputHAT(HIDP_VALUE_CAPS *caps, unsigned short valueID, InputButton *up, InputButton *down, InputButton *right, InputButton *left);
			virtual ~InputHAT();
			virtual void SetValue(unsigned long value) override;
			bool IsButton() override;
			bool IsHAT() override;

			USAGE Page;
			direction Direction;
			unsigned short ValueID;
			unsigned long MinValue;
			unsigned long MaxValue;
			bool MovedThisFrame;

		protected:
			InputButton *Up;
			InputButton *Down;
			InputButton *Right;
			InputButton *Left;
		};
	}
}
