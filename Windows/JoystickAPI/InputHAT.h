#pragma once
#include <Windows.h>
#include <hidsdi.h>
#include <hidpi.h>
#include "InputControl.h"

namespace TChapman500
{
	namespace JoystickAPI
	{
		class InputHAT : public InputControl
		{
		public:
			InputHAT();
			InputHAT(HIDP_VALUE_CAPS *caps, unsigned short valueID);
			virtual ~InputHAT();
			virtual void SetValue(unsigned long value) override;

			enum class direction
			{
				Top,
				TopRight,
				Right,
				BottomRight,
				Bottom,
				BottomLeft,
				Left,
				TopLeft,
				None
			};

			USAGE Page;
			direction Direction;
			unsigned short ValueID;
			unsigned long MinValue;
			unsigned long MaxValue;
			bool MovedThisFrame;
		};
	}
}
