#pragma once

#include <ctime>
#include <iostream>
#include <sstream>

#include "../../libs/discord_sdk/x64/include/discord.h"
#include "../sniffer/MemoryInfo.h"
#include "../util/Logger.h"

class DiscordRPC
{
public:
	bool clearedActivity = false;

	DiscordRPC(std::shared_ptr<Logger> logger);
	void DiscordRPC::Init();
	void DiscordRPC::SetActivity(ParsedMemoryInfo& memoryInfo);
	void DiscordRPC::ClearActivity();

private:
	std::shared_ptr<Logger> logger = nullptr;
	ParsedMemoryInfo oldMemoryInfo{};
	std::time_t startTime = 0;
	bool failed = false;

	discord::Core* core = nullptr;
	discord::User user{};
};

