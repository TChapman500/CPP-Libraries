#pragma once
#include <string>
#include <vector>
#include <memory>

namespace TChapman500 {
class INIKey;
class INISection;
class INIFile
{
public:
	INIFile(std::wstring path);
	INIFile(std::wstring &path);
	~INIFile();


	long long GetInt(std::wstring &section, std::wstring &key, long long def);
	long long GetInt(std::wstring &section, std::wstring key, long long def);
	long long GetInt(std::wstring section, std::wstring &key, long long def);
	long long GetInt(std::wstring section, std::wstring key, long long def);

	unsigned long long GetUInt(std::wstring &section, std::wstring &key, unsigned long long def);
	unsigned long long GetUInt(std::wstring &section, std::wstring key, unsigned long long def);
	unsigned long long GetUInt(std::wstring section, std::wstring &key, unsigned long long def);
	unsigned long long GetUInt(std::wstring section, std::wstring key, unsigned long long def);

	double GetFloat(std::wstring &section, std::wstring &key, double def);
	double GetFloat(std::wstring &section, std::wstring key, double def);
	double GetFloat(std::wstring section, std::wstring &key, double def);
	double GetFloat(std::wstring section, std::wstring key, double def);

	std::wstring GetString(std::wstring &section, std::wstring &key);
	std::wstring GetString(std::wstring &section, std::wstring key);
	std::wstring GetString(std::wstring section, std::wstring &key);
	std::wstring GetString(std::wstring section, std::wstring key);

private:
	std::vector<std::unique_ptr<INISection>> Sections;

	long long _GetInt(std::wstring &section, std::wstring &key, long long def);
	unsigned long long _GetUInt(std::wstring &section, std::wstring &key, unsigned long long def);
	double _GetFloat(std::wstring &section, std::wstring &key, double def);
	std::wstring _GetString(std::wstring &section, std::wstring &key);
	void _Initialize(std::wstring &path);
};
}
