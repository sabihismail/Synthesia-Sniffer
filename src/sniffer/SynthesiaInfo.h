#pragma once

#include <string>
#include <tuple>
#include <vector>

namespace Synthesia
{
	template<class T1, class T2>
	struct MemoryTuple
	{
		T1 first;
		T2 second;
	};

	struct MemoryFetch
	{
		int address;
		std::vector<int> offsets;
	};

	struct MemoryMap
	{
		Synthesia::MemoryTuple<int, const char*> version; // vXX.X
		Synthesia::MemoryTuple<int, const char*> revision; // rXXXXX
		Synthesia::MemoryFetch menu; // tracks
		Synthesia::MemoryFetch songPaused; // Song Paused
		Synthesia::MemoryFetch gameMode; // Watch and Listen Only
		Synthesia::MemoryFetch songInfo; // test for song errors (easiest)
		Synthesia::MemoryFetch totalTime; // totalTime
		Synthesia::MemoryFetch currentTime; // currentTime
		Synthesia::MemoryFetch notes; // notesLabel
	};

	// 64-bit is the only supported version now
	static MemoryMap v10p9
	{
		Synthesia::MemoryTuple<int, const char*> { 0x00511978, "10.9" },
		Synthesia::MemoryTuple<int, const char*> { 0x0050C8E0, "r5903" },
		Synthesia::MemoryFetch { 0x0050DB90 },
		Synthesia::MemoryFetch { 0x005133C0, std::vector<int> { 0x8, 0xE8, 0x0, 0x20, 0x218, 0x50, 0x1F8 } },
		Synthesia::MemoryFetch { 0x0042BD38, std::vector<int> { 0xA4, 0x0, 0xA8, 0x144, 0x4, 0x140 } },
		Synthesia::MemoryFetch { 0x0042BD34},
		Synthesia::MemoryFetch { 0x0042BD38, std::vector<int> { 0xA4, 0x0, 0x208, 0x0, 0x164, 0x88 } },
		Synthesia::MemoryFetch { 0x0042BD38, std::vector<int> { 0xA4, 0x0, 0xD8, 0x19C, 0x0, 0x1C4 } },
		Synthesia::MemoryFetch { 0x0042BD38, std::vector<int> { 0xA4, 0x0, 0x1A4, 0x33C, 0x1A8, 0xC8 } },
	};

	static std::vector<MemoryMap> MEMORY_MAPPING
	{
		v10p9
	};

	static const wchar_t* PROCESS_NAME = _T("Synthesia.exe");

	static std::map<std::string, MenuType> MENU_MAPPING
	{
		{ "title", MenuType::MAIN_MENU },
		{ "library", MenuType::TRACK_SELECTION },
		{ "tracks", MenuType::GAME_MODE_SELECTION },
		{ "freeplay", MenuType::FREE_PLAY },
	};

	static std::map<std::string, GameModeFlag> GAME_MODE_MAPPING
	{
		{ "Watch and Listen Only", GameModeFlag::WATCH_AND_LISTEN_ONLY },
		{ "Melody Practice", GameModeFlag::PRACTICE_THE_MELODY },
		{ "Rhythm Practice", GameModeFlag::PRACTICE_THE_RHYTHM },
		{ "Recital", GameModeFlag::SONG_RECITAL },
		{ "Right Hand", GameModeFlag::RIGHT_HAND },
		{ "Left Hand", GameModeFlag::LEFT_HAND },
		{ "Both Hands", GameModeFlag::RIGHT_HAND | GameModeFlag::LEFT_HAND },
	};
}
