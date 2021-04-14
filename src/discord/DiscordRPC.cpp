#include "DiscordRPC.h"

constexpr auto CLIENT_ID = 712374934864659046;
constexpr auto SYNTHESIA_IMAGE = "synthesia";

DiscordRPC::DiscordRPC()
{
    auto result = discord::Core::Create(CLIENT_ID, DiscordCreateFlags_Default, &core);

    if (result != discord::Result::Ok)
    {
        return;
    }

    core->SetLogHook(discord::LogLevel::Debug, [this](discord::LogLevel lvl, const char* str)
        {

        });

    core->UserManager().OnCurrentUserUpdate.Connect([this]() 
        {
            core->UserManager().GetCurrentUser(&user);
        });

    core->RunCallbacks();

    successfulStart = 1;
}

void DiscordRPC::SetActivity(ParsedSongInfo& songInfo)
{
    discord::Activity activity{};
    activity.SetType(discord::ActivityType::Playing);
    activity.SetDetails(songInfo.song.title.c_str());
    activity.SetState(songInfo.song.artist.c_str());
    activity.SetInstance(true);

    auto& timeStamp = activity.GetTimestamps();
    timeStamp.SetStart(0);

    std::time_t epochTime = std::time(nullptr);
    std::int64_t endTimeStamp = (epochTime + songInfo.timeTotal) - songInfo.timeCurrent;
    timeStamp.SetEnd(endTimeStamp);

    auto& assets = activity.GetAssets();
    assets.SetLargeImage(SYNTHESIA_IMAGE);
    assets.SetLargeText(songInfo.synthesiaVersionInfo.c_str());
    assets.SetSmallImage(SYNTHESIA_IMAGE);
    assets.SetSmallText("Gamer");

    core->ActivityManager().UpdateActivity(activity, [this](discord::Result result)
        {

        });

    core->RunCallbacks();
}
