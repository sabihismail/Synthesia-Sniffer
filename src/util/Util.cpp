#include "Util.h"

std::string Util::SafeStr(std::string& str, std::string& defaultStr)
{
	if (str.empty() || str[0] == '\0')
	{
		return defaultStr;
	}

	return str;
}

std::map<std::string, std::string> Util::ParseStringByFormat(std::string& str, std::string& format)
{
	std::vector<std::string> splt;
	std::vector<std::string> params;
	std::string tmpStr;
	int varHit = 0;
	for (char c : format)
	{
		if (c == '$')
		{
			if (!tmpStr.empty())
			{
				splt.push_back(std::string(tmpStr));

				tmpStr.clear();
			}

			varHit = 1;
		}

		if (c == ' ' && varHit)
		{
			auto s = std::string(tmpStr);

			if (tmpStr[0] == '$')
			{
				params.push_back(s.substr(1));
			}

			splt.push_back(s);

			tmpStr.clear();
			varHit = 0;
		}

		tmpStr += c;
	}

	if (tmpStr[0] == '$')
	{
		params.push_back(tmpStr.substr(1));
	}

	if (!tmpStr.empty())
	{
		splt.push_back(tmpStr);

		tmpStr.clear();
	}

	std::string regexStr;
	for (std::string s : splt)
	{
		if (s[0] == '$')
		{
			regexStr += "(.*)";
		}
		else
		{
			regexStr += s;
		}
	}

	std::regex regex(regexStr);
	std::smatch matches;

	if (!std::regex_search(str, matches, regex))
	{
		throw std::exception("Invalid format.");
	}

	std::map<std::string, std::string> mapping;
	for (size_t i = 1; i < matches.size(); i++)
	{
		std::string param = params[i - 1];

		if (param == "$?")
		{
			continue;
		}

		std::string match = matches[i].str();

		mapping[param] = match;
	}

	return mapping;
}

std::string Util::ReplaceAllString(std::string subject, const std::string& search, const std::string& replace)
{
	size_t pos = 0;

	while ((pos = subject.find(search, pos)) != std::string::npos) 
	{
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}

	return subject;
}

void Util::ReplaceAllStringInPlace(std::string& subject, const std::string& search, const std::string& replace)
{
	size_t pos = 0;

	while ((pos = subject.find(search, pos)) != std::string::npos) 
	{
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

int Util::IsNumber(std::string& str)
{
	for (char c : str)
	{
		if (c < '0' || c > '9')
		{
			return false;
		}
	}

	return true;
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

uint32_t Util::SafeStringToUInt32(std::string& str, int assumption)
{
	if (IsNumber(str))
	{
		return static_cast<uint32_t>(std::stoul(str));
	}

	return assumption;
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

std::vector<std::string> Util::SplitString(std::string& str, std::string& delimiter, bool toTrim)
{
	std::string duplicated(str);

	std::vector<std::string> lst;

	size_t pos = 0;
	while ((pos = duplicated.find(delimiter)) != std::string::npos)
	{
		std::string tmp = duplicated.substr(0, pos);

		if (toTrim)
		{
			Trim(tmp);
		}

		lst.push_back(tmp);

		duplicated.erase(0, pos + delimiter.length());
	}

	if (!duplicated.empty())
	{
		if (toTrim)
		{
			Trim(duplicated);
		}

		lst.push_back(duplicated);
	}

	return lst;
}

std::vector<std::string> Util::SplitString(std::string& str, char delimiter, bool toTrim)
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
