#pragma once

#include "../../libs/discord_sdk/include/discord.h"
#include "../sniffer/MemoryInfo.h"
#include "../util/Logger.h"
#include <ctime>
#include <sstream>
#include <iostream>

class DiscordRPC
{
public:
	int successfulStart = 0;

	DiscordRPC(std::shared_ptr<Logger> logger);
	void DiscordRPC::SetActivity(ParsedMemoryInfo& memoryInfo);

private:
	std::shared_ptr<Logger> logger = nullptr;
	ParsedMemoryInfo oldMemoryInfo{};
	std::time_t startTime = 0;

	discord::Core* core{};
	discord::User user{};
};

