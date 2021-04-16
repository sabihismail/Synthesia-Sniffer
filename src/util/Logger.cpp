#include "Logger.h"

constexpr auto LOG_FILE = "log.txt";
constexpr auto LOG_ERROR_FILE = "logError.txt";

std::mutex logFuncMutex;

std::mutex logMutex;
std::mutex logErrorMutex;

Logger::Logger() {}

void Logger::Log(std::string& inStr, const char* className, LogType logType)
{
	std::mutex* mutex = &logMutex;
	std::string file = LOG_FILE;

	if (logType == LogType::LOG_ERROR)
	{
		mutex = &logErrorMutex;
		file = LOG_ERROR_FILE;
	}
	else if (logType == LogType::LOG_INFO)
	{
		mutex = &logMutex;
		file = LOG_FILE;
	}

	auto time = std::time(nullptr);
	auto tm = *std::localtime(&time);

	std::ostringstream ossTime{};
	ossTime << std::put_time(&tm, "%d/%m/%Y %H:%M:%S");
	std::string timeStr = ossTime.str();

	std::string str = "[" + timeStr + "] " + className + " - " + inStr;

	mutex->lock();

	std::ofstream outFile;
	outFile.open(file, std::ios::out | std::ios_base::app);

	outFile << str << std::endl;

	mutex->unlock();

	std::string uiStr = "[" + timeStr + "] " + inStr;

	logFuncMutex.lock();
	for (auto const& func : onLogCallbacks)
	{
		func(logType, uiStr);
	}
	logFuncMutex.unlock();
}

void Logger::LogDifference(const char* variable, int oldVal, int newVal, bool isEnum, LogType logType)
{
	std::string oldConv = isEnum ? std::bitset<8>(oldVal).to_string() : std::to_string(oldVal);
	std::string newConv = isEnum ? std::bitset<8>(newVal).to_string() : std::to_string(newVal);

	LogDifference(variable, oldConv, newConv, logType);
}

void Logger::LogDifference(const char* variable, std::string& oldVal, std::string& newVal, LogType logType)
{
	std::string str = " from " + oldVal + " to " + newVal;

	Log(str, variable, logType);
}

void Logger::OnLog(std::function<void(LogType&, std::string&)> func)
{
	logFuncMutex.lock();

	onLogCallbacks.push_back(func);

	logFuncMutex.unlock();
}
