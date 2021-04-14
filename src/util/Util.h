#pragma once

#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <string>

class Util
{
public:
	static int IsNumber(std::string& str);
	static int Util::CheckArray(char* arr, int length);
	static int SafeStringToInt(std::string& str, int assumption = 0);
	static int SafeStringToInt(char* arr, int length, int assumption = 0);
	static uint64_t Util::SafeStringToUInt64(std::string& str, int assumption = 0);
	static int ValidString(std::string& str);
	static std::vector<std::string> Util::SplitString(std::string& str, char delimiter, int toTrim = 0);
	static void LTrim(std::string& s);
	static void RTrim(std::string& s);
	static void Trim(std::string& s);
	static std::string LTrimCopy(std::string s);
	static std::string RTrimCopy(std::string s);
	static std::string TrimCopy(std::string s);
};
