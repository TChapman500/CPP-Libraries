#include "InputDevice.h"
#include <Windows.h>

#include "InputButton.h"
#include "InputAxis.h"
#include "InputHAT.h"

namespace TChapman500
{
	namespace JoystickAPI
	{
		InputDevice::InputDevice(HANDLE device)
		{
			Device = device;

			UINT dataSize = 0;
			std::string devName;
			GetRawInputDeviceInfoA(device, RIDI_DEVICENAME, nullptr, &dataSize);
			devName.resize(dataSize);
			GetRawInputDeviceInfoA(device, RIDI_DEVICENAME, (void *)devName.data(), &dataSize);

			DeviceName.resize(126);
			HANDLE HIDHandle = CreateFileA(devName.data(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
			if (HIDHandle)
			{
				BOOLEAN productString = HidD_GetProductString(HIDHandle, (void *)DeviceName.data(), sizeof(wchar_t) * 126);
				CloseHandle(HIDHandle);
			}
		}

		InputDevice::~InputDevice()
		{
		}
	}
}
