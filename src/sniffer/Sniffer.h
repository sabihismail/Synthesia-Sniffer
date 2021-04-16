#pragma once

#include "PEProcess.h"
#include "../util/Util.h"
#include "../util/Settings.h"
#include "../util/Logger.h"
#include "../util/VariableMessageBox.h"
#include "./MemoryInfo.h"
#include <QMainWindow>
#include <QMessageBox>
#include <qstring.h>
#include <qabstractbutton.h>
#include <thread>
#include <functional>
#include <iostream>
#include <regex>

struct SynthesiaVersion
{
	std::string version;
	std::string revision;
};

class Sniffer
{
public:
	int running = 1;

	Sniffer(Settings* settings, Logger* logger);
	~Sniffer();
	void Init();
	void OnUpdate(std::function<void(ParsedMemoryInfo&)> callback);
	void OnGUIRequest(std::function<void(VariableMessageBox&)> callback);

private:
	Settings* settings = nullptr;
	Logger* logger = nullptr;

	SynthesiaVersion synthesiaVersion{};
	std::string version;
	std::string revision;

	std::vector<std::function<void(ParsedMemoryInfo&)>> callbacks;
	std::function<void(VariableMessageBox&)> guiRequestCallback;
	MemoryInfoStructs songInfoTemp{};
	ParsedMemoryInfo oldSongInfo{};
	ParsedMemoryInfo songInfo{};

	void Sniffer::CheckVersion();
	int Sniffer::GetCurrentInformation(PEProcess& process);
	void Sniffer::FormatSongInfo(PEProcess& process);
	void Sniffer::CompareMemoryInfoDifferences();
	uint64_t Sniffer::ConvertTimeToUInt64(std::string& time);
	ParsedMemoryInfo::Song Sniffer::ParseSongName(std::string& file);
};
