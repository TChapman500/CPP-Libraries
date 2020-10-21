#pragma once
#include <string>

namespace TChapman500
{
	namespace JoystickAPI
	{
		class InputControl
		{
		public:
			InputControl();
			virtual ~InputControl();
			virtual void SetValue(unsigned long value) = 0;

			void SetName(std::wstring name);

			std::wstring ControlName;
		};

	}
}
