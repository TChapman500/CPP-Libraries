#include "INIKey.h"

namespace TChapman500 {
INIKey::INIKey(std::wstring &line)
{
	const wchar_t *whitespace = L" \t\n\r\f\v";
	std::wstring preComment = line.substr(0, line.find(L';'));
	if (preComment.empty()) return;

	preComment.erase(0, preComment.find_first_not_of(whitespace));
	preComment.erase(preComment.find_last_not_of(whitespace) + 1);

	size_t pos = line.find(L'=');
	if (pos == std::wstring::npos) return;

	Key = preComment.substr(0, pos);
	Key.erase(Key.find_last_not_of(whitespace) + 1);

	Value = preComment.substr(pos + 1);
	Value.erase(0, Value.find_first_not_of(whitespace));
}

INIKey::~INIKey() {}
void INIKey::SetValue(std::wstring value) { Value = value; }
void INIKey::SetValue(std::wstring &value) { Value = value; }
std::wstring INIKey::GetName() { return Key; }
std::wstring INIKey::GetValue() { return Value; }
}