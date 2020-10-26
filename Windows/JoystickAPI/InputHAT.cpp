#include "InputHAT.h"
#include "InputButton.h"

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

			Up = nullptr;
			Down = nullptr;
			Right = nullptr;
			Left = nullptr;
		}

		InputHAT::InputHAT(HIDP_VALUE_CAPS *caps, unsigned short valueID, InputButton *up, InputButton *down, InputButton *right, InputButton *left)
		{
			ValueID = valueID;
			MinValue = caps->LogicalMin;
			MaxValue = caps->LogicalMax;
			Page = caps->UsagePage;
			Direction = direction::None;
			MovedThisFrame = false;

			Up = up;
			Down = down;
			Right = right;
			Left = left;
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

			if (Direction == direction::Up || Direction == direction::UpLeft || Direction == direction::UpRight) Up->SetValue(1);
			else Up->SetValue(0);

			if (Direction == direction::Right || Direction == direction::UpRight || Direction == direction::DownRight) Right->SetValue(1);
			else Right->SetValue(0);

			if (Direction == direction::Down || Direction == direction::DownLeft || Direction == direction::DownRight) Down->SetValue(1);
			else Down->SetValue(0);

			if (Direction == direction::Left || Direction == direction::UpLeft || Direction == direction::DownLeft) Left->SetValue(1);
			else Left->SetValue(0);
		}
		bool InputHAT::IsButton() { return false; }
		bool InputHAT::IsHAT() { return true; }
	}
}