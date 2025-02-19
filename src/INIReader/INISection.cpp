#include "INISection.h"
#include "INIKey.h"

namespace TChapman500 {
INISection::INISection(std::wstring &line)
{
	const wchar_t *whitespace = L" \t\n\r\f\v";
	Name = line.substr(1, line.find(L']') - 1);
	size_t comment = line.find(L';');
	if (comment != std::wstring::npos) Name = line.substr(0, comment);
	Name.erase(0, Name.find_first_not_of(whitespace));
	Name.erase(Name.find_last_not_of(whitespace) + 1);
}

INISection::~INISection() {}

std::wstring INISection::GetName() { return Name; }

bool INISection::AddKey(std::unique_ptr<INIKey> &newKey)
{
	if (newKey->GetName().empty()) return false;

	for (const std::unique_ptr<INIKey> &key : Keys)
	{
		if (key->GetName() == newKey->GetName())
		{
			key->SetValue(newKey->GetValue());
			return true;
		}
	}
	Keys.push_back(std::move(newKey));
	return true;
}

long long INISection::GetInt(std::wstring &key, long long def)
{
	for (size_t i = 0; i < Keys.size(); i++)
	{
		if (Keys[i]->GetName() == key)
			return wcstoll(Keys[i]->GetValue().c_str(), nullptr, 0);
	}
	return def;
}

unsigned long long INISection::GetUInt(std::wstring &key, unsigned long long def)
{
	for (size_t i = 0; i < Keys.size(); i++)
	{
		if (Keys[i]->GetName() == key)
			return wcstoull(Keys[i]->GetValue().c_str(), nullptr, 0);
	}
	return def;
}

double INISection::GetFloat(std::wstring &key, double def)
{
	for (size_t i = 0; i < Keys.size(); i++)
	{
		if (Keys[i]->GetName() == key)
			return wcstod(Keys[i]->GetValue().c_str(), nullptr);
	}
	return def;
}

std::wstring INISection::GetString(std::wstring &key)
{
	for (size_t i = 0; i < Keys.size(); i++)
	{
		if (Keys[i]->GetName() == key)
			return Keys[i]->GetValue();
	}
	return std::wstring();
}
}