#include "DiscordRPC.h"

constexpr auto CLIENT_ID = 712374934864659046;

DiscordRPC::DiscordRPC()
{
    core = *new discord::Core*();

    auto result = discord::Core::Create(CLIENT_ID, DiscordCreateFlags_Default, &core);

    if (result != discord::Result::Ok)
    {
        return;
    }

    successfulStart = 1;
}

void DiscordRPC::SetActivity()
{
    discord::Activity activity{};
    activity.SetState("Testing");
    activity.SetDetails("Fruit Loops");
    core->ActivityManager().UpdateActivity(activity, nullptr);
}
