#pragma once

#include "PEProcess.h"
#include "../util/Util.h"
#include "../util/Settings.h"
#include "../util/VariableMessageBox.h"
#include "./SongInfo.h"
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

	Sniffer(Settings* settings);
	void Init();
	void OnUpdate(std::function<void(ParsedSongInfo&)> callback);
	void OnGUIRequest(std::function<void(VariableMessageBox&)> callback);

private:
	Settings* settings = nullptr;

	SynthesiaVersion synthesiaVersion{};
	std::string version;
	std::string revision;

	std::vector<std::function<void(ParsedSongInfo&)>> callbacks;
	std::function<void(VariableMessageBox&)> guiRequestCallback;
	MemorySongInfo songInfoTemp{};
	ParsedSongInfo songInfo{};

	void Sniffer::CheckVersion();
	int Sniffer::GetCurrentInformation(PEProcess& process);
	void Sniffer::FormatSongInfo(PEProcess& process);
	uint64_t Sniffer::ConvertTimeToUInt64(std::string& time);
	ParsedSongInfo::Song Sniffer::ParseSongName(std::string& file);
};
