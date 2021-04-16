#pragma once

#include <fstream>
#include <string>
#include <filesystem>
#include <mutex>
#include <functional>
#include <ctime>
#include <bitset>
#include <sstream>

#define nameof(name) #name

template <typename E>
constexpr auto ToUnderlying(E e) noexcept
{
	return static_cast<std::underlying_type_t<E>>(e);
}

enum class LogType
{
	LOG_DEBUG,
	LOG_INFO,
	LOG_ERROR
};

class Logger
{
public:
	Logger();
	void Log(std::string& str, const char* className, LogType logType = LogType::LOG_DEBUG);
	template<class T>
	void Log(std::string& str, T t, LogType logType = LogType::LOG_DEBUG);
	template<class T>
	void Log(const char* str, T t, LogType logType = LogType::LOG_DEBUG);
	template<class T>
	void LogException(std::exception& e, T t, std::string str = "");
	void LogDifference(const char* variable, std::string& oldVal, std::string& newVal, LogType logType = LogType::LOG_DEBUG);
	void LogDifference(const char* variable, int oldVal, int newVal, bool isEnum = true, LogType logType = LogType::LOG_DEBUG);
	void OnLog(std::function<void(LogType&, std::string&)> func);

private:
	std::vector<std::function<void(LogType&, std::string&)>> onLogCallbacks{};
};

template<class T>
void Logger::Log(std::string& str, T t, LogType logType)
{
	Log(str, typeid(T).name(), logType);
}

template<class T>
void Logger::Log(const char* str, T t, LogType logType)
{
	Log(std::string(str), t, logType);
}

template<class T>
void Logger::LogException(std::exception& e, T t, std::string str)
{
	std::string s = std::string(e.what()) + "\n" + str;

	Log(s, t, LogType::LOG_ERROR);
}

