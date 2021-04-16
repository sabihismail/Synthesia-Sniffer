#pragma once

#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <string>
#include <map>
#include <regex>

class Util
{
public:
	template<class T1, class T2>
	static T2 SafeMapRetrieval(std::map<T1, T2> mapping, T1, T2);
	static std::string ReplaceAllString(std::string subject, const std::string& search, const std::string& replace);
	static void ReplaceAllStringInPlace(std::string& subject, const std::string& search, const std::string& replace);
	static std::map<std::string, std::string> Util::ParseStringByFormat(std::string& str, std::string& format);
	static int IsNumber(std::string& str);
	static int Util::CheckArray(char* arr, int length);
	static int SafeStringToInt(std::string& str, int assumption = 0);
	static int SafeStringToInt(char* arr, int length, int assumption = 0);
	static uint32_t Util::SafeStringToUInt32(std::string& str, int assumption = 0);
	static uint64_t Util::SafeStringToUInt64(std::string& str, int assumption = 0);
	static int ValidString(std::string& str);
	static std::vector<std::string> Util::SplitString(std::string& str, char delimiter, bool toTrim = false);
	static std::vector<std::string> Util::SplitString(std::string& str, std::string& delimiter, bool toTrim = false);
	static void LTrim(std::string& s);
	static void RTrim(std::string& s);
	static void Trim(std::string& s);
	static std::string LTrimCopy(std::string s);
	static std::string RTrimCopy(std::string s);
	static std::string TrimCopy(std::string s);
};

template<class T1, class T2>
T2 Util::SafeMapRetrieval(std::map<T1, T2> mapping, T1 key, T2 defaultValue)
{
	if (mapping.find(key) == mapping.end())
	{
		return defaultValue;
	}

	return mapping[key];
}
