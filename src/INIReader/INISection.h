#pragma once
#include <string>
#include <memory>
#include <vector>

namespace TChapman500 {
class INIKey;
class INISection
{
public:
	INISection(std::wstring &line);
	~INISection();

	std::wstring GetName();

	bool AddKey(std::unique_ptr<INIKey> &newKey);

	long long GetInt(std::wstring &key, long long def);
	unsigned long long GetUInt(std::wstring &key, unsigned long long def);
	double GetFloat(std::wstring &key, double def);
	std::wstring GetString(std::wstring &key);


private:
	std::wstring Name;
	std::vector<std::unique_ptr<INIKey>> Keys;
};
}
