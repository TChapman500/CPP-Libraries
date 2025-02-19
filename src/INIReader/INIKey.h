#pragma once
#include <string>
#include <memory>
#include <vector>

namespace TChapman500 {
class INIKey
{
public:
	INIKey(std::wstring &line);
	virtual ~INIKey();

	void SetValue(std::wstring value);
	void SetValue(std::wstring &value);
	std::wstring GetName();
	std::wstring GetValue();

private:
	std::wstring Key;
	std::wstring Value;
};
}
