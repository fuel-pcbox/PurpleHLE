#pragma once
#include "types.h"

// boost? what's that?

inline std::string BeforeFirst(const std::string& str, char ch)
{
	u32 loc = str.find_first_of(ch);
	return str.substr(0, loc);
}

inline std::string AfterFirst(const std::string& str, char ch)
{
	u32 loc = str.find_first_of(ch);
	if (loc >= str.size() - 1)
		return std::string(); //empty
	else
		return str.substr(loc + 1);
}

inline std::string BeforeLast(const std::string& str, char ch)
{
	u32 loc = str.find_last_of(ch);
	return str.substr(0, loc);
}

inline std::string AfterLast(const std::string& str, char ch)
{
	u32 loc = str.find_last_of(ch);
	if (loc >= str.size() - 1)
		return std::string(); //empty
	else
		return str.substr(loc + 1);
}

inline bool StartsWith(const std::string& str, const std::string& substr)
{
	u32 loc = str.find(substr);
	return loc == 0;
}

inline bool EndsWith(const std::string& str, const std::string& substr)
{
	u32 loc = str.rfind(substr);
	return loc == str.size() - substr.size();
}

inline std::string ReplaceChar(const std::string& str, char ch, const std::string& with) //with must not contain ch's
{
	std::string newstr = str;
	while (true)
	{
		int off = newstr.find_first_of(ch);
		if (off == std::string::npos)
			return newstr;

		newstr = newstr.replace(off, 1, with);			
	}
}

inline std::string MakeZStr(const char* chars, int maxAmt)
{
	std::string str = std::string(chars, maxAmt);
	u32 end = str.find('\0');
	return str.substr(0, end);
}