#include "InputAxis.h"

namespace TChapman500
{
	namespace JoystickAPI
	{
		InputAxis::InputAxis(HIDP_VALUE_CAPS *caps)
		{
			MinValue = caps->LogicalMin & 0xFFFF;
			MaxValue = caps->LogicalMax & 0xFFFF;
			if (caps->LogicalMax < 0)
			{
				Bits = caps->BitSize / 2;
				MinValue >>= Bits;
				MaxValue >>= Bits;
				UseHalf = true;
			}
			else
			{
				Bits = caps->BitSize;
				UseHalf = false;
			}

			Page = caps->UsagePage;
			Usage = caps->Range.UsageMin;

			CenterRelative = 0.0f;
			EndRelative = 0.5f;
			RawValue = 0;
		}

		InputAxis::~InputAxis()
		{
		}

		void InputAxis::SetValue(unsigned long value)
		{
			if (UseHalf)
			{
				value >>= Bits;
			}

			RawValue = value;
			EndRelative = (float)RawValue / (float)MaxValue;

			unsigned long halfValue = MaxValue / 2;

			// Odd Number of States
			if (MaxValue % 2 == 0)
			{
				if (RawValue > halfValue)
				{
					CenterRelative = (float)(RawValue - halfValue) / (float)halfValue;
				}
				else if (RawValue < halfValue)
				{
					CenterRelative = ((float)RawValue / (float)halfValue) - 1.0f;
				}
				else CenterRelative = 0.0f;
			}
			// Even Number of States
			else
			{
				unsigned short overHalf = halfValue + 1;
				if (RawValue >= overHalf)
				{
					CenterRelative = (float)(RawValue - overHalf) / (float)halfValue;
				}
				else
				{
					CenterRelative = ((float)RawValue / (float)halfValue) - 1.0f;
				}
			}

		}

		bool InputAxis::IsButton() { return false; }
		bool InputAxis::IsHAT() { return false; }
	}
}