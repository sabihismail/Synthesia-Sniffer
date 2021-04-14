#include "Util.h"

int Util::IsNumber(std::string& str)
{
	return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

int Util::SafeStringToInt(std::string& str, int assumption)
{
	if (IsNumber(str))
	{
		return std::stoi(str);
	}

	return assumption;
}

int Util::SafeStringToInt(char* arr, int length, int assumption)
{
	int res = CheckArray(arr, length);

	if (res)
	{
		return assumption;
	}

	return SafeStringToInt(std::string(arr), assumption);
}

uint64_t Util::SafeStringToUInt64(std::string& str, int assumption)
{
	if (IsNumber(str))
	{
		return std::stoull(str);
	}

	return assumption;
}

int Util::CheckArray(char* arr, int length)
{
	for (int i = 0; i < length; i++)
	{
		if (arr[i] < 0 || arr[i] > 255)
		{
			return 0;
		}

		if (!std::isdigit(arr[i]))
		{
			return 0;
		}
	}

	return 1;
}

int Util::ValidString(std::string& str) 
{
	return !str.empty();
}

std::vector<std::string> Util::SplitString(std::string& str, char delimiter, int toTrim)
{
	std::vector<std::string> lst;

	std::stringstream ss(str);
	std::string item;
	while (std::getline(ss, item, delimiter))
	{
		if (toTrim)
		{
			Trim(item);
		}

		lst.push_back(item);
	}

	return lst;
}

void Util::LTrim(std::string& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) 
		{
			return !std::isspace(ch);
		}));
}

void Util::RTrim(std::string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
		{
			return !std::isspace(ch);
		}).base(), s.end());
}

void Util::Trim(std::string& s)
{
	LTrim(s);
	RTrim(s);
}

std::string Util::LTrimCopy(std::string s)
{
	LTrim(s);
	return s;
}

std::string Util::RTrimCopy(std::string s)
{
	RTrim(s);
	return s;
}

std::string Util::TrimCopy(std::string s)
{
	Trim(s);
	return s;
}
