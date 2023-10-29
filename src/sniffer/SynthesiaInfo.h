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
		int offsetRun;
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

	static MemoryMap v10p6
	{
		Synthesia::MemoryTuple<int, const char*> { 0x00376C30, "10.6" },
		Synthesia::MemoryTuple<int, const char*> { 0x00376C38, "r5425" },
		Synthesia::MemoryFetch { 0x004203E8 },
		Synthesia::MemoryFetch { 0x0041EE38, 280, std::vector<int> { 0xA4, 0x0, 0xD8, 0x64, 0x168, 0x168 } },
		Synthesia::MemoryFetch { 0x0041EE38, 0, std::vector<int> { 0xA4, 0x0, 0xA8, 0x128, 0x14, 0x118 } },
		Synthesia::MemoryFetch { 0x0041EE34, 264 },
		Synthesia::MemoryFetch { 0x0041EE38, 364, std::vector<int> { 0xA4, 0x0, 0xC4, 0x1B0, 0x0, 0x180 } },
		Synthesia::MemoryFetch { 0x0041EE38, 364, std::vector<int> { 0xA4, 0x0, 0xD0, 0x168, 0x1CC, 0x84 } },
		Synthesia::MemoryFetch { 0x0041EE38, 364, std::vector<int> { 0xA4, 0x0, 0xC4, 0x124, 0x19C, 0x180 } },
	};

	static MemoryMap v10p7
	{
		Synthesia::MemoryTuple<int, const char*> { 0x0041012C, "10.7" },
		Synthesia::MemoryTuple<int, const char*> { 0x00410114, "r5567" },
		Synthesia::MemoryFetch { 0x0042D388 },
		Synthesia::MemoryFetch { 0x0042BD38, 320, std::vector<int> { 0xA4, 0x0, 0x1C4, 0x168, 0xE4, 0x8 } },
		Synthesia::MemoryFetch { 0x0042BD38, 140, std::vector<int> { 0xA4, 0x0, 0xA8, 0x144, 0x4, 0x140 } },
		Synthesia::MemoryFetch { 0x0042BD34, 264 },
		Synthesia::MemoryFetch { 0x0042BD38, 400, std::vector<int> { 0xA4, 0x0, 0x208, 0x0, 0x164, 0x88 } },
		Synthesia::MemoryFetch { 0x0042BD38, 400, std::vector<int> { 0xA4, 0x0, 0xD8, 0x19C, 0x0, 0x1C4 } },
		Synthesia::MemoryFetch { 0x0042BD38, 400, std::vector<int> { 0xA4, 0x0, 0x1A4, 0x33C, 0x1A8, 0xC8 } },
	};

	static MemoryMap v10p9
	{
		Synthesia::MemoryTuple<int, const char*> { 0x00393530, "10.9" },
		Synthesia::MemoryTuple<int, const char*> { 0x0050C8E0, "r5903" },
		Synthesia::MemoryFetch { 0x0042D388 },
		Synthesia::MemoryFetch { 0x0042BD38, 320, std::vector<int> { 0xA4, 0x0, 0x1C4, 0x168, 0xE4, 0x8 } },
		Synthesia::MemoryFetch { 0x0042BD38, 140, std::vector<int> { 0xA4, 0x0, 0xA8, 0x144, 0x4, 0x140 } },
		Synthesia::MemoryFetch { 0x0042BD34, 264 },
		Synthesia::MemoryFetch { 0x0042BD38, 400, std::vector<int> { 0xA4, 0x0, 0x208, 0x0, 0x164, 0x88 } },
		Synthesia::MemoryFetch { 0x0042BD38, 400, std::vector<int> { 0xA4, 0x0, 0xD8, 0x19C, 0x0, 0x1C4 } },
		Synthesia::MemoryFetch { 0x0042BD38, 400, std::vector<int> { 0xA4, 0x0, 0x1A4, 0x33C, 0x1A8, 0xC8 } },
	};

	static std::vector<MemoryMap> MEMORY_MAPPING
	{
		v10p6,
		v10p7,
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
