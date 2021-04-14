#pragma once

#include "../../libs/discord_sdk/include/discord.h"
#include "../sniffer/SongInfo.h"
#include <ctime>
#include <iostream>
#include<QDebug>

class DiscordRPC
{
public:
	int successfulStart = 0;

	DiscordRPC();
	void DiscordRPC::SetActivity(ParsedSongInfo& songInfo);

private:
	discord::Core* core{};
	discord::User user{};
	std::string s;
};

