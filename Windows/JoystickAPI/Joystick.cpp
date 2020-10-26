#include <string.h>
#include <string>
#include "Joystick.h"
#include "InputButton.h"
#include "InputAxis.h"
#include "InputHAT.h"
#include "CustomHAT.h"

namespace TChapman500
{
	namespace JoystickAPI
	{
		Joystick::Joystick(HANDLE device) : InputDevice(device)
		{
			UINT dataSize = 0;
			GetRawInputDeviceInfoA(device, RIDI_PREPARSEDDATA, nullptr, &dataSize);
			_HIDP_PREPARSED_DATA *preparsed = (_HIDP_PREPARSED_DATA *)new char[dataSize];
			GetRawInputDeviceInfoA(device, RIDI_PREPARSEDDATA, preparsed, &dataSize);

			HIDP_CAPS caps;

			HidP_GetCaps(preparsed, &caps);
			std::vector<HIDP_BUTTON_CAPS> buttonList;
			buttonList.resize(caps.NumberInputButtonCaps);
			HidP_GetButtonCaps(HidP_Input, buttonList.data(), &caps.NumberInputButtonCaps, preparsed);

			int buttonCount = buttonList[0].Range.UsageMax - buttonList[0].Range.UsageMin + 1;
			ButtonPage = buttonList[0].UsagePage;
			_ButtonList.resize(buttonCount);
			ButtonList.resize(buttonCount);
			for (int i = 0; i < buttonCount; i++)
			{
				InputButton *newButton = new InputButton((unsigned short)i);

				// Format Button String
				wchar_t name[64];
				swprintf_s(name, 64, L"Button %d", i + 1);
				newButton->SetName(name);

				_ButtonList[i] = newButton;
				ButtonList[i] = newButton;
			}

			std::vector<HIDP_VALUE_CAPS> axisList;
			axisList.resize(caps.NumberInputValueCaps);
			HidP_GetValueCaps(HidP_Input, axisList.data(), &caps.NumberInputValueCaps, preparsed);

			for (int i = 0, a = 0; i < axisList.size(); i++)
			{
				InputAxis *newAxis = new InputAxis(&axisList[i]);


				_ValueList.push_back(newAxis);

				// HAT Switch
				if (axisList[i].Range.UsageMin == 0x39)
				{
					InputButton *hatUp = new InputButton((unsigned short)(buttonCount));
					hatUp->SetName(L"HAT Switch (Up)");
					ButtonList.push_back(hatUp);

					InputButton *hatRight = new InputButton((unsigned short)(buttonCount + 1));
					hatRight->SetName(L"HAT Switch (Right)");
					ButtonList.push_back(hatRight);

					InputButton *hatDown = new InputButton((unsigned short)(buttonCount + 2));
					hatDown->SetName(L"HAT Switch (Down)");
					ButtonList.push_back(hatDown);

					InputButton *hatLeft = new InputButton((unsigned short)(buttonCount + 3));
					hatLeft->SetName(L"HAT Switch (Left)");
					ButtonList.push_back(hatLeft);

					InputHAT *newHAT = new InputHAT(&axisList[i], (unsigned short)i, hatUp, hatDown, hatRight, hatLeft);

					// Set HAT Name
					newHAT->SetName(L"HAT Switch");

					HATList.push_back(newHAT);


				}
				else if (axisList[i].Range.UsageMin > 0x20)
				{
					// Set Axis Name
					wchar_t name[64];
					switch (axisList[i].Range.UsageMin)
					{
					case 0x30:
						swprintf_s(name, 64, L"X Axis");
						break;
					case 0x31:
						swprintf_s(name, 64, L"Y Axis");
						break;
					case 0x32:
						swprintf_s(name, 64, L"Z Axis");
						break;
					case 0x33:
						swprintf_s(name, 64, L"Rx Axis");
						break;
					case 0x34:
						swprintf_s(name, 64, L"Ry Axis");
						break;
					case 0x35:
						swprintf_s(name, 64, L"Rz Axis");
						break;
					case 0x36:
						swprintf_s(name, 64, L"Slider");
						break;
					case 0x37:
						swprintf_s(name, 64, L"Dial");
						break;
					case 0x38:
						swprintf_s(name, 64, L"Wheel");
						break;
					default:
						swprintf_s(name, 64, L"Axis %d", a + 1);
						break;
					}
					a++;
					newAxis->SetName(name);


					AxisList.push_back(newAxis);
				}
			}
		}

		Joystick::~Joystick()
		{
			for (int i = 0; i < _ButtonList.size(); i++) delete _ButtonList[i];
			for (int i = 0; i < _ValueList.size(); i++) delete _ValueList[i];
			for (int i = 0; i < HATList.size(); i++) delete HATList[i];
		}

		void Joystick::ProcessInputs(RAWINPUT *rawInput)
		{
			UINT dataSize;
			_HIDP_PREPARSED_DATA *preparsed;
			GetRawInputDeviceInfoA(Device, RIDI_PREPARSEDDATA, nullptr, &dataSize);
			preparsed = (_HIDP_PREPARSED_DATA *)new char[dataSize];
			GetRawInputDeviceInfoA(Device, RIDI_PREPARSEDDATA, preparsed, &dataSize);

			std::vector<USAGE> usages;
			ULONG buttonCount = _ButtonList.size();
			usages.resize(buttonCount);

			HidP_GetUsages(HidP_Input, ButtonPage, 0, usages.data(), &buttonCount, preparsed, (char *)rawInput->data.hid.bRawData, rawInput->data.hid.dwSizeHid);

			// Get all button states.
			std::vector<bool> buttonStates;
			buttonStates.resize(_ButtonList.size());
			for (int i = 0; i < buttonCount; i++)
			{
				if (usages[i] > 0) buttonStates[usages[i] - 1] = true;
			}

			// Update all button states.
			for (int i = 0; i < _ButtonList.size(); i++)
			{
				_ButtonList[i]->SetValue(buttonStates[i]);
			}

			// Update all axis states.
			for (int i = 0, h = 0; i < _ValueList.size(); i++)
			{
				ULONG value;

				HidP_GetUsageValue(HidP_Input, _ValueList[i]->Page, 0, _ValueList[i]->Usage, &value, preparsed, (PCHAR)rawInput->data.hid.bRawData, rawInput->data.hid.dwSizeHid);
				if (_ValueList[i]->Usage == 0x39)
				{
					int test = 2;
					HATList[h]->SetValue(value);
					h++;
				}
				else _ValueList[i]->SetValue(value);
			}

			delete[] preparsed;
		}

		void Joystick::AddCustomHAT(unsigned short up, unsigned short down, unsigned short right, unsigned short left, std::wstring name)
		{
			//for (int i = 0, x = ButtonList.size(); i < x; i++)
			//{
			//	if (ButtonList[i]->ButtonID == up || ButtonList[i]->ButtonID == down || ButtonList[i]->ButtonID == right || ButtonList[i]->ButtonID == left)
			//	{
			//		ButtonList.erase(ButtonList.begin() + i);
			//		x = ButtonList.size();
			//		i--;
			//	}
			//}

			HIDP_VALUE_CAPS valueCap;
			ZeroMemory(&valueCap, sizeof(HIDP_VALUE_CAPS));
			valueCap.BitSize = 4;
			valueCap.LogicalMin = 1;
			valueCap.LogicalMax = 8;
			valueCap.UsagePage = -1;
			valueCap.Range.UsageMin = 0x39;

			CustomHAT *newHAT = new CustomHAT(_ButtonList[up], _ButtonList[down], _ButtonList[right], _ButtonList[left]);

			// Set name if available.
			if (name.size() > 0) newHAT->SetName(name.data());

			_ValueList.push_back(new InputAxis(&valueCap));
			HATList.push_back(newHAT);
		}
	}
}
