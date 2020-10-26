#pragma once
#include <string>

namespace TChapman500
{
	namespace JoystickAPI
	{
		enum class direction
		{
			Up,
			UpRight,
			Right,
			DownRight,
			Down,
			DownLeft,
			Left,
			UpLeft,
			None
		};

		class InputControl
		{
		public:
			InputControl();
			virtual ~InputControl();
			virtual void SetValue(unsigned long value) = 0;
			virtual bool IsButton() = 0;
			virtual bool IsHAT() = 0;

			void SetName(std::wstring name);

			std::wstring ControlName;
		};

	}
}
