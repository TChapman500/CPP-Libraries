#include "InputHAT.h"

namespace TChapman500
{
	namespace JoystickAPI
	{
		InputHAT::InputHAT()
		{
			ValueID = -1;
			MinValue = 1;
			MaxValue = 8;
			Page = -1;
			Direction = direction::None;
			MovedThisFrame = false;
		}

		InputHAT::InputHAT(HIDP_VALUE_CAPS *caps, unsigned short valueID)
		{
			ValueID = valueID;
			MinValue = caps->LogicalMin;
			MaxValue = caps->LogicalMax;
			Page = caps->UsagePage;
			Direction = direction::None;
			MovedThisFrame = false;
		}

		InputHAT::~InputHAT()
		{
		}

		void InputHAT::SetValue(unsigned long value)
		{
			direction newDirection = (direction)(value - MinValue);
			if (newDirection < (direction)0) newDirection = direction::None;

			if (newDirection != Direction) MovedThisFrame = true;
			else MovedThisFrame = false;

			Direction = newDirection;
		}
	}
}