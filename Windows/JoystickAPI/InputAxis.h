#pragma once
#include "InputControl.h"
#include <Windows.h>
#include <hidsdi.h>
#include <hidpi.h>

namespace TChapman500
{
	namespace JoystickAPI
	{
		class InputAxis : public InputControl
		{
		public:
			InputAxis(HIDP_VALUE_CAPS *caps);
			virtual ~InputAxis();
			virtual void SetValue(unsigned long value) override;

			USAGE Page;
			USAGE Usage;
			unsigned short Bits;
			bool UseHalf;
			unsigned long MinValue;
			unsigned long MaxValue;
			unsigned long RawValue;
			float CenterRelative;
			float EndRelative;
		};
	}
}
