#pragma once

#include <Windows.h>
#include <stdint.h>
#include <string>

struct MemorySongInfo
{
	struct MemorySongInfoS1
	{
		char unknown1[264];
		uint64_t currentTimeLong;
		uint32_t notesHit;
		uint32_t errors;
		char unknown2[144];
		uint32_t songPlayCount;
		char unknown3[104];
		LPVOID addressToSongFilePath;
	} songInfoRead;
};

struct ParsedSongInfo
{
	uint32_t notesMax;
	uint32_t notesHit;
	uint32_t errors;
	uint32_t songPlayCount;
	uint32_t timeCurrent;
	uint32_t timeTotal;
	std::string songFilePath;
	std::string synthesiaVersionInfo;

	struct Song
	{
		std::string title;
		std::string artist;
	} song;
};
