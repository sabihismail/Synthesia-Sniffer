#include "DiscordRPC.h"

constexpr auto CLIENT_ID = 712374934864659046;
constexpr auto SYNTHESIA_IMAGE = "synthesia";

DiscordRPC::DiscordRPC(std::shared_ptr<Logger> logger)
{
    this->logger = logger;

    auto result = discord::Core::Create(CLIENT_ID, DiscordCreateFlags_Default, &core);

    if (result != discord::Result::Ok)
    {
        return;
    }

    core->SetLogHook(discord::LogLevel::Debug, [this, logger](discord::LogLevel lvl, const char* str)
        {
            logger->Log(str, this, LogType::LOG_ERROR);
        });

    core->UserManager().OnCurrentUserUpdate.Connect([this, logger]() 
        {
            core->UserManager().GetCurrentUser(&user);

            std::string str = "Username: " + std::string(user.GetUsername()) + " - ID: " + std::to_string(user.GetId());

            logger->Log(str, this, LogType::LOG_INFO);
        });

    core->RunCallbacks();

    successfulStart = 1;
}

void DiscordRPC::SetActivity(ParsedMemoryInfo& songInfo)
{
    this->clearedActivity = false;

    try
    {
        discord::Activity activity{};
        activity.SetType(discord::ActivityType::Playing);
        activity.SetInstance(true);

        auto& timeStamp = activity.GetTimestamps();
        auto& assets = activity.GetAssets();

        assets.SetSmallImage(SYNTHESIA_IMAGE);
        assets.SetLargeImage(SYNTHESIA_IMAGE);
        assets.SetLargeText(songInfo.synthesiaVersionInfo.c_str());

        if (oldMemoryInfo.menuType == MenuType::UNKNOWN) 
        {
            startTime = std::time(nullptr);
        }

        switch (songInfo.menuType)
        {
        case MenuType::MAIN_MENU:
            activity.SetDetails("Browsing");
            activity.SetState("Main Menu");

            timeStamp.SetStart(startTime);
            break;

        case MenuType::TRACK_SELECTION:
            activity.SetDetails("Browsing");
            activity.SetState("Looking for a Song");

            timeStamp.SetStart(startTime);
            break;

        case MenuType::FREE_PLAY:
            activity.SetDetails("Experimenting");
            activity.SetState("Free Play");

            timeStamp.SetStart(startTime);
            break;

        case MenuType::GAME_MODE_SELECTION:
            activity.SetDetails("Browsing");
            activity.SetState("Choosing Game Mode");

            timeStamp.SetStart(startTime);
            break;

        case MenuType::IN_GAME:
            if (songInfo.gameMode & GameModeFlag::WATCH_AND_LISTEN_ONLY)
            {
                activity.SetDetails("Observing");
                activity.SetState("Spectate Mode");
            }
            else
            {
                activity.SetDetails(songInfo.song.title.c_str());
                activity.SetState(songInfo.song.artist.c_str());

                double accuracy = 1.0;
                if (!songInfo.notesHit == 0)
                {
                    accuracy -= ((double)songInfo.errors / (songInfo.notesHit + songInfo.errors));
                }
                accuracy *= 100;

                std::ostringstream stream;
                stream << "Accuracy: ";
                stream << std::fixed << std::setprecision(2) << accuracy << "%";

                assets.SetSmallText(stream.str().c_str());
                assets.SetLargeText(songInfo.gameModeStr.c_str());
            }

            std::int64_t endTimeStamp = (std::time(nullptr) + songInfo.timeTotal) - songInfo.timeCurrent;
            timeStamp.SetEnd(endTimeStamp);

            timeStamp.SetStart(0);

            break;
        }

        core->ActivityManager().UpdateActivity(activity, [this](discord::Result result)
            {
                if (result == discord::Result::Ok) return;

                std::string str = "UpdateActivity failed: " + ToUnderlying(result);

                logger->Log(str, this, LogType::LOG_ERROR);
            });

        core->RunCallbacks();

        memcpy(&oldMemoryInfo, &songInfo, sizeof(ParsedMemoryInfo));
    }
    catch (std::exception e)
    {
        logger->LogException(e, nameof(SetActivity));
    }
}

void DiscordRPC::ClearActivity()
{
    if (this->clearedActivity) return;

    core->ActivityManager().ClearActivity([this](discord::Result result)
    {
        this->clearedActivity = result == discord::Result::Ok;
    });

    core->RunCallbacks();

    // Used to reset timer once again
    oldMemoryInfo.menuType = MenuType::UNKNOWN;
}
