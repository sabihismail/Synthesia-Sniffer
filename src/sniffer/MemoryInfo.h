#pragma once

#include "../util/Util.h"
#include <Windows.h>
#include <stdint.h>
#include <string>
#include <map>
#include <sstream>

#define nameof(name) #name

enum class MenuType
{
	UNKNOWN,
	MAIN_MENU,
	TRACK_SELECTION,
	FREE_PLAY,
	GAME_MODE_SELECTION,
	IN_GAME,
	SETTINGS,
};

enum class GameModeFlag
{
	NONE = 0,
	WATCH_AND_LISTEN_ONLY = 1,
	PRACTICE_THE_MELODY = 2,
	PRACTICE_THE_RHYTHM = 4,
	SONG_RECITAL = 8,
	LEFT_HAND = 16,
	RIGHT_HAND = 32,
	UNKNOWN = 64,
};

inline bool operator&(GameModeFlag a, GameModeFlag b)
{
	return static_cast<int>(a) & static_cast<int>(b);
}

inline GameModeFlag operator|(GameModeFlag a, GameModeFlag b)
{
	return static_cast<GameModeFlag>(static_cast<int>(a) | static_cast<int>(b));
}

inline GameModeFlag& operator|=(GameModeFlag& a, GameModeFlag b)
{
	return a = a | b;
}

struct MemoryInfoStructs
{
	struct S1
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

struct ParsedMemoryInfo
{
	std::map<MenuType, std::string> MENU_TYPE_TO_STR
	{
		{ MenuType::MAIN_MENU, "Main Menu" },
		{ MenuType::TRACK_SELECTION, "Track Selection" },
		{ MenuType::FREE_PLAY, "Free Play" },
		{ MenuType::GAME_MODE_SELECTION, "Game Mode Selection" },
		{ MenuType::IN_GAME, "In Game" },
		{ MenuType::SETTINGS, "Settings" },
	};

	uint32_t notesMax;
	uint32_t notesHit;
	uint32_t errors;
	uint32_t songPlayCount;
	uint32_t timeCurrent;
	uint32_t timeTotal;
	std::string songFilePath;
	std::string synthesiaVersionInfo;
	std::string gameModeStr;
	MenuType menuType{};
	GameModeFlag gameMode{};

	struct Song
	{
		std::string title;
		std::string artist;
	} song;

	std::string ToString()
	{
		std::ostringstream ss;

		ss << nameof(synthesiaVersionInfo) << " - " << synthesiaVersionInfo << "\n\n";
		ss << nameof(songFilePath) << " - " << songFilePath << "\n\n";
		ss << nameof(menuType) << " - " << Util::SafeMapRetrieval(MENU_TYPE_TO_STR, menuType, std::string("")) << "\n\n";
		ss << nameof(gameModeStr) << " - " << Util::SafeStr(gameModeStr) << "\n\n";
		ss << nameof(song.title) << " - " << Util::SafeStr(song.title) << "\n\n";
		ss << nameof(song.artist) << " - " << Util::SafeStr(song.artist) << "\n\n";
		ss << nameof(songPlayCount) << " - " << songPlayCount << "\n\n";
		ss << nameof(timeCurrent) << "/" << nameof(timeTotal) << " - " << timeCurrent << "/" << timeTotal << "\n\n";
		ss << nameof(notesHit) << "/" << nameof(notesHit) << " - " << notesHit << "/" << notesHit << "\n\n";
		ss << nameof(errors) << " - " << errors;

		return ss.str();
	}
};
