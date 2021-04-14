#pragma once

#include "PEProcess.h"
#include "../util/Util.h"
#include <QMessageBox>
#include <QMainWindow>
#include <thread>

struct SongInfoTemp
{
	struct SongInfoRead
	{
		char unknown1[264];
		uint64_t currentTimeLong;
		uint32_t notesHit;
		uint32_t errors;
		char unknown2[144];
		uint32_t songPlayCount;
		char unknown3[104];
		LPVOID addressToSongFilePath;
		/*char unknown4[1068];
		char songRunTime[14];
		char unknown5[398];
		char maxNotes[12];
		char unknown6[2432];
		char currentTime[16];
		char unknown7[3248];
		char songLength[16];*/
	} songInfoRead;
};


struct SongInfo
{
	uint32_t notesHit;
	uint32_t errors;
	uint32_t songPlayCount;
	std::string songFilePath;
	uint32_t timeCurrent;
	uint32_t timeTotal;
	int notesMax;
};

class Sniffer
{
public:
	int running = 1;

	Sniffer();
	void Init();
	void OnUpdate(std::function<void(SongInfo*)> callback);

private:
	QMainWindow* window;

	std::string version;
	std::string revision;

	std::vector<std::function<void(SongInfo*)>> callbacks;
	SongInfoTemp songInfoTemp{};
	SongInfo songInfo{};

	void Sniffer::CheckVersion();
	int Sniffer::GetCurrentInformation(PEProcess process);
	void Sniffer::FormatSongInfo(PEProcess process);
	uint64_t Sniffer::ConvertTimeToUInt64(std::string& time);
};
