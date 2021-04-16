#include "Sniffer.h"

wchar_t* PROCESS_NAME = _T("Synthesia.exe");

std::vector<std::tuple<const char*, const char*>> VALID_SYNTHESIA_VERSIONS
{
    { "10.6", "r5425" }
};

std::vector<std::tuple<int, int>> SYNTHESIA_VERSION_ADDRESSES
{
    { 0x376C30, 0x376C38 }
};

std::map<std::string, MenuType> MENU_MAPPING
{
    { "title", MenuType::MAIN_MENU },
    { "library", MenuType::TRACK_SELECTION },
    { "tracks", MenuType::GAME_MODE_SELECTION },
    { "freeplay", MenuType::FREE_PLAY },
};

std::map<std::string, GameModeFlag> GAME_MODE_MAPPING
{
    { "Watch and Listen Only", GameModeFlag::WATCH_AND_LISTEN_ONLY },
    { "Melody Practice", GameModeFlag::PRACTICE_THE_MELODY },
    { "Rhythm Practice", GameModeFlag::PRACTICE_THE_RHYTHM },
    { "Recital", GameModeFlag::SONG_RECITAL },
    { "Right Hand", GameModeFlag::RIGHT_HAND },
    { "Left Hand", GameModeFlag::LEFT_HAND },
    { "Both Hands", GameModeFlag::RIGHT_HAND | GameModeFlag::LEFT_HAND },
};

Sniffer::Sniffer(Settings* settings, Logger* logger)
{
    this->settings = settings;
    this->logger = logger;
}

Sniffer::~Sniffer()
{
    running = 0;
}

void Sniffer::Init()
{
    while (running)
    {
        PEProcess process = PEProcess::WaitForProcessAvailability(PROCESS_NAME, &running);
        process.SetModule(PROCESS_NAME);

        logger->Log("Found process", this, LogType::LOG_DEBUG);

        for (std::tuple<int, int> address : SYNTHESIA_VERSION_ADDRESSES)
        {
            int versionAddress = std::get<0>(address);
            int revisionAddress = std::get<1>(address);

            version = process.ReadMemoryString(versionAddress);
            revision = process.ReadMemoryString(revisionAddress);
        }

        CheckVersion();

        logger->Log("Completed version check", this, LogType::LOG_DEBUG);

        while (synthesiaVersion.version.empty())
        {
            Sleep(100);
        }

        while (process.StillAlive() && running)
        {
            memcpy(&oldSongInfo, &songInfo, sizeof(ParsedMemoryInfo));

            int success = GetCurrentInformation(process);

            if (success)
            {
                FormatSongInfo(process);

                CompareMemoryInfoDifferences();

                for (std::function<void(ParsedMemoryInfo&)> callback : callbacks)
                {
                    callback(songInfo);
                }
            }

            Sleep(100);
        }
    }
}

void Sniffer::OnUpdate(std::function<void(ParsedMemoryInfo&)> callback)
{
    callbacks.push_back(callback);
}

void Sniffer::OnGUIRequest(std::function<void(VariableMessageBox&)> callback)
{
    guiRequestCallback = callback;
}

void Sniffer::CompareMemoryInfoDifferences()
{
    ParsedMemoryInfo* o = &oldSongInfo;
    ParsedMemoryInfo* n = &songInfo;

    if (n->gameModeStr.empty())
    {
        n->gameModeStr = o->gameModeStr;
    }

    if (n->gameMode == GameModeFlag::NONE && !(o->gameMode & GameModeFlag::NONE))
    {
        n->gameMode = o->gameMode;
        n->gameModeStr = o->gameModeStr;
    }

    if (n->menuType == MenuType::IN_GAME)
    {
        n->gameMode = o->gameMode;
        n->gameModeStr = o->gameModeStr;
    }

    if (n->menuType != o->menuType)
    {
        logger->LogDifference(nameof(songInfo->menuType), std::to_string(static_cast<int>(o->menuType)), std::to_string(static_cast<int>(n->menuType)));
    }

    if (n->gameMode != o->gameMode)
    {
        logger->LogDifference(nameof(songInfo->gameMode), static_cast<int>(o->gameMode), static_cast<int>(n->gameMode), true);
    }

    if (n->songFilePath != o->songFilePath)
    {
        logger->LogDifference(nameof(songInfo->songFilePath), o->songFilePath, n->songFilePath);
        logger->LogDifference(nameof(songInfo->song.title), o->song.title, n->song.title);
        logger->LogDifference(nameof(songInfo->song.artist), o->song.artist, n->song.artist);
    }
}

void Sniffer::FormatSongInfo(PEProcess& process)
{
    std::string songFilePath = process.ReadMemoryString(songInfoTemp.songInfoRead.addressToSongFilePath, MAX_PATH * 2);

    songInfo.notesHit = songInfoTemp.songInfoRead.notesHit;
    songInfo.errors = songInfoTemp.songInfoRead.errors;
    songInfo.songPlayCount = songInfoTemp.songInfoRead.songPlayCount;
    songInfo.songFilePath = songFilePath;

    songInfo.song = ParseSongName(songFilePath);
}

ParsedMemoryInfo::Song Sniffer::ParseSongName(std::string& file)
{
    ParsedMemoryInfo::Song song{};

    try
    {
        std::string formatted = Util::ReplaceAllString(file, "\\", "/");

        auto splt = Util::SplitString(formatted, '/', true);
        std::string filename = splt.at(splt.size() - 1);

        auto mapping = Util::ParseStringByFormat(filename, settings->songFormat);

        auto title = Util::SafeMapRetrieval(mapping, std::string("title"), std::string(""));
        auto artist = Util::SafeMapRetrieval(mapping, std::string("artist"), std::string(""));

        Util::ReplaceAllStringInPlace(artist, ".mid", "");
        Util::ReplaceAllStringInPlace(artist, ".midi", "");

        Util::ReplaceAllStringInPlace(title, ".mid", "");
        Util::ReplaceAllStringInPlace(title, ".midi", "");

        song.title = title;
        song.artist = artist;
    }
    catch (std::exception e)
    {
        logger->LogException(e, this, nameof(StartSniffer));
    }

    return song;
}

int Sniffer::GetCurrentInformation(PEProcess& process)
{
    try 
    {
        std::string menuAddress = process.ReadMemoryString(0x004203E8, 16);
        if (Util::ValidString(menuAddress))
        {
            MenuType menuType = Util::SafeMapRetrieval(MENU_MAPPING, menuAddress, MenuType::UNKNOWN);

            int songPausedOffsets[] = { 0xA4, 0x0, 0xD8, 0x64, 0x168, 0x168 };
            LPVOID songPausedAddress = process.ReadMemoryAddressChain(0x0041EE38, songPausedOffsets, sizeof(songPausedOffsets));

            if (songPausedAddress != 0x0)
            {
                std::string songPausedStr = process.ReadMemoryString(songPausedAddress, 12, nullptr, 280);

                if (songPausedStr == "Song Paused")
                {
                    menuType = MenuType::IN_GAME;
                }
            }

            GameModeFlag gameMode = GameModeFlag::NONE;
            if (menuType == MenuType::GAME_MODE_SELECTION)
            {
                int gameModeOffsets[] = { 0xA4, 0x0, 0xA8, 0x128, 0x14, 0x118 };
                LPVOID gameModeAddress = process.ReadMemoryAddressChain(0x0041EE38, gameModeOffsets, sizeof(gameModeOffsets));
                std::string gameModeStr = process.ReadMemoryString(gameModeAddress, 32);
                auto splt = Util::SplitString(gameModeStr, std::string(" • "), true);

                for (size_t i = 0; i < splt.size(); i++) 
                {
                    GameModeFlag flag = Util::SafeMapRetrieval(GAME_MODE_MAPPING, splt[i], GameModeFlag::UNKNOWN);

                    if (i == 0)
                    {
                        gameMode = flag;
                    }
                    else
                    {
                        gameMode |= flag;
                    }

                    if (flag == GameModeFlag::UNKNOWN)
                    {
                        std::string str = "Invalid parsing game mode " + gameModeStr + " - " + splt[i];

                        logger->Log(str, nameof(GetCurrentInformation), LogType::LOG_ERROR);
                    }
                }

                songInfo.gameModeStr = gameModeStr;
            }

            songInfo.menuType = menuType;
            songInfo.gameMode = gameMode;
        }

        LPVOID songInfoAddress = process.ReadMemoryAddress(0x0041EE34);

        songInfoTemp.songInfoRead = process.ReadMemoryStruct<MemoryInfoStructs::S1>(songInfoAddress);

        int totalTimeOffsets[] = { 0xA4, 0x0, 0xC4, 0x1B0, 0x0, 0x180 };
        LPVOID totalTimeAddress = process.ReadMemoryAddressChain(0x0041EE38, totalTimeOffsets, sizeof(totalTimeOffsets));
        std::string totalTime = process.ReadMemoryString(totalTimeAddress, 16, nullptr, 364);

        if (Util::ValidString(totalTime))
        {
            songInfo.timeTotal = ConvertTimeToUInt64(totalTime);
        }

        int currentTimeOffsets[] = { 0xA4, 0x0, 0xD0, 0x168, 0x1CC, 0x84 };
        LPVOID currentTimeAddress = process.ReadMemoryAddressChain(0x0041EE38, currentTimeOffsets, sizeof(currentTimeOffsets));
        std::string currentTime = process.ReadMemoryString(currentTimeAddress, 16, nullptr, 364);

        if (Util::ValidString(currentTime))
        {
            songInfo.timeCurrent = ConvertTimeToUInt64(currentTime);
        }

        int notesOffsets[] = { 0xA4, 0x0, 0xC4, 0x124, 0x19C, 0x180 };
        LPVOID notesAddress = process.ReadMemoryAddressChain(0x0041EE38, notesOffsets, sizeof(notesOffsets));
        std::string notes = process.ReadMemoryString(notesAddress, 16, nullptr, 364);

        if (Util::ValidString(notes))
        {
            auto splt = Util::SplitString(notes, '/', true);
            songInfo.notesMax = Util::SafeStringToUInt32(splt.at(1));
        }
    }
    catch (std::exception e)
    {
        logger->LogException(e, this, nameof(StartSniffer));

        return 0;
    }

    return 1;
}

uint64_t Sniffer::ConvertTimeToUInt64(std::string& time)
{
    uint64_t total = 0;

    std::string sub;
    for (auto c : time)
    {
        if (isdigit(c))
        {
            sub += c;
            continue;
        }

        uint64_t val = Util::SafeStringToUInt64(sub);

        if (c == ':')
        {
            total += val * 60;
        }
        else if (c == '.')
        {
            total += val;
        }

        sub = "";
    }

    return total;
}

void Sniffer::CheckVersion()
{
    int valid = 0;
    for (int i = 0; i < VALID_SYNTHESIA_VERSIONS.size(); i++) 
    {
        std::tuple<const char*, const char*> elem = VALID_SYNTHESIA_VERSIONS[i];

        const char* ver = std::get<0>(elem);
        const char* rev = std::get<1>(elem);

        if (version == ver && revision == rev)
        {
            valid = 1;
            break;
        }
    }

    std::function<void()> proceed = [&]()
    {
        synthesiaVersion =
        {
            version,
            revision
        };

        songInfo.synthesiaVersionInfo = "Synthesia v" + version + "/" + revision;

        logger->Log(songInfo.synthesiaVersionInfo, this, LogType::LOG_DEBUG);
    };

    if (!valid)
    {
        if (guiRequestCallback)
        {
            std::function<void()> cancel = [&]() 
            {
                exit(-1);
            };

            VariableMessageBox obj
            {
                "Sorry, SynthesiaSniffer doesn't support this version of Synthesia. Please make a request or update your Synthesia.",
                std::vector<const char*> { "Continue", "Exit" },
                std::vector<std::function<void()>> { proceed, cancel }
            };

            guiRequestCallback(obj);
        }
        else
        {
            exit(-1);
        }
    }
    else
    {
        proceed();
    }
}
