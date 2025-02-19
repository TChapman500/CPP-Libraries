#include "INIFile.h"
#include <fstream>

#include "INISection.h"
#include "INIKey.h"

namespace TChapman500 {
INIFile::INIFile(std::wstring path) { _Initialize(path); }
INIFile::INIFile(std::wstring &path) { _Initialize(path); }
INIFile::~INIFile() {}

long long INIFile::GetInt(std::wstring &section, std::wstring &key, long long def) { return _GetInt(section, key, def); }
long long INIFile::GetInt(std::wstring &section, std::wstring key, long long def) { return _GetInt(section, key, def); }
long long INIFile::GetInt(std::wstring section, std::wstring &key, long long def) { return _GetInt(section, key, def); }
long long INIFile::GetInt(std::wstring section, std::wstring key, long long def) { return _GetInt(section, key, def); }

unsigned long long INIFile::GetUInt(std::wstring &section, std::wstring &key, unsigned long long def) { return _GetUInt(section, key, def); }
unsigned long long INIFile::GetUInt(std::wstring &section, std::wstring key, unsigned long long def) { return _GetUInt(section, key, def); }
unsigned long long INIFile::GetUInt(std::wstring section, std::wstring &key, unsigned long long def) { return _GetUInt(section, key, def); }
unsigned long long INIFile::GetUInt(std::wstring section, std::wstring key, unsigned long long def) { return _GetUInt(section, key, def); }

double INIFile::GetFloat(std::wstring &section, std::wstring &key, double def) { return _GetFloat(section, key, def); }
double INIFile::GetFloat(std::wstring &section, std::wstring key, double def) { return _GetFloat(section, key, def); }
double INIFile::GetFloat(std::wstring section, std::wstring &key, double def) { return _GetFloat(section, key, def); }
double INIFile::GetFloat(std::wstring section, std::wstring key, double def) { return _GetFloat(section, key, def); }

std::wstring INIFile::GetString(std::wstring &section, std::wstring &key) { return _GetString(section, key); }
std::wstring INIFile::GetString(std::wstring &section, std::wstring key) { return _GetString(section, key); }
std::wstring INIFile::GetString(std::wstring section, std::wstring &key) { return _GetString(section, key); }
std::wstring INIFile::GetString(std::wstring section, std::wstring key) { return _GetString(section, key); }

long long INIFile::_GetInt(std::wstring &section, std::wstring &key, long long def)
{
	for (size_t i = 0; i < Sections.size(); i++)
	{
		if (Sections[i]->GetName() == section)
			return Sections[i]->GetInt(key, def);
	}
	return def;
}

unsigned long long INIFile::_GetUInt(std::wstring &section, std::wstring &key, unsigned long long def)
{
	for (size_t i = 0; i < Sections.size(); i++)
	{
		if (Sections[i]->GetName() == section)
			return Sections[i]->GetUInt(key, def);
	}
	return def;
}

double INIFile::_GetFloat(std::wstring &section, std::wstring &key, double def)
{
	for (size_t i = 0; i < Sections.size(); i++)
	{
		if (Sections[i]->GetName() == section)
			return Sections[i]->GetFloat(key, def);
	}
	return def;
}

std::wstring INIFile::_GetString(std::wstring &section, std::wstring &key)
{
	for (size_t i = 0; i < Sections.size(); i++)
	{
		if (Sections[i]->GetName() == section)
			return Sections[i]->GetString(key);
	}
	return std::wstring();
}

void INIFile::_Initialize(std::wstring &path)
{
	std::wfstream file(path, std::ios::in);
	if (!file.is_open()) return;

	std::wstring currLine;
	std::unique_ptr<INISection> currSection;
	while (file)
	{
		std::getline(file, currLine);
		if (currLine.empty()) continue;

		// Remove leading whitespace
		const wchar_t *whitespace = L" \t\n\r\f\v";
		currLine.erase(0, currLine.find_first_not_of(whitespace));

		// Line is empty or contains only a comment
		if (currLine.empty() || currLine[0] == L';') continue;

		// We are not yet in a section
		if (!currSection && currLine[0] != L'[') continue;

		// Create a new section
		if (currLine[0] == L'[')
		{
			if (currSection) Sections.push_back(std::move(currSection));
			currSection = std::make_unique<INISection>(currLine);
			continue;
		}

		// Add the new key
		std::unique_ptr<INIKey> newKey = std::make_unique<INIKey>(currLine);
		currSection->AddKey(newKey);
	}

	// End of file
	if (currSection) Sections.push_back(std::move(currSection));
}
}