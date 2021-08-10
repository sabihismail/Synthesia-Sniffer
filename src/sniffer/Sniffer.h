#pragma once

#include "PEProcess.h"
#include "../util/Util.h"
#include "../util/Settings.h"
#include "../util/Logger.h"
#include "../util/VariableMessageBox.h"
#include "./MemoryInfo.h"
#include "./SynthesiaInfo.h"
#include <QMainWindow>
#include <QMessageBox>
#include <qstring.h>
#include <qabstractbutton.h>
#include <thread>
#include <functional>
#include <iostream>
#include <regex>

class Sniffer
{
public:
	int running = 1;
	bool processAlive = true;

	Sniffer(std::shared_ptr<Settings> settings, std::shared_ptr<Logger> logger);
	~Sniffer();
	void Init();
	void OnUpdate(std::function<void(ParsedMemoryInfo&)> callback);
	void OnGUIRequest(std::function<void(VariableMessageBox&)> callback);

private:
	std::shared_ptr<Settings> settings = nullptr;
	std::shared_ptr<Logger> logger = nullptr;

	Synthesia::MemoryMap memoryMapping{};
	std::string version;
	std::string revision;

	std::vector<std::function<void(ParsedMemoryInfo&)>> callbacks;
	std::function<void(VariableMessageBox&)> guiRequestCallback;
	MemoryInfoStructs songInfoTemp{};
	ParsedMemoryInfo oldSongInfo{};
	ParsedMemoryInfo songInfo{};

	void Sniffer::UpdateGUI();
	void Sniffer::CheckVersion(PEProcess& process);
	int Sniffer::GetCurrentInformation(PEProcess& process);
	void Sniffer::FormatSongInfo(PEProcess& process);
	void Sniffer::CompareMemoryInfoDifferences();
	uint64_t Sniffer::ConvertTimeToUInt64(std::string& time);
	ParsedMemoryInfo::Song Sniffer::ParseSongName(std::string& file, PEProcess& process);
};
