#pragma once

#include "../../libs/discord_sdk/include/discord.h"

class DiscordRPC
{
public:
	int successfulStart = 0;

	DiscordRPC();
	void DiscordRPC::SetActivity();

private:
	discord::Core* core = nullptr;
};

