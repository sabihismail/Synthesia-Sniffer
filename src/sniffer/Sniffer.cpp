#include "Sniffer.h"

Sniffer::Sniffer(std::shared_ptr<Settings> settings, std::shared_ptr<Logger> logger)
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
        PEProcess process = PEProcess::WaitForProcessAvailability(Synthesia::PROCESS_NAME, &running);
        process.SetModule(Synthesia::PROCESS_NAME);

        logger->Log("Found process", this, LogType::LOG_INFO);

        CheckVersion(process);
        processAlive = true;

        logger->Log("Completed version check", this, LogType::LOG_DEBUG);

        while (process.StillAlive() && running)
        {
            memcpy(&oldSongInfo, &songInfo, sizeof(ParsedMemoryInfo));

            int success = GetCurrentInformation(process);

            if (success)
            {
                FormatSongInfo(process);

                CompareMemoryInfoDifferences();

                UpdateGUI();
            }

            Sleep(100);
        }

        logger->Log("Process was closed", this, LogType::LOG_INFO);

        processAlive = false;
        songInfo = {};

        UpdateGUI();
    }
}

void Sniffer::UpdateGUI()
{
    for (std::function<void(ParsedMemoryInfo&)> callback : callbacks)
    {
        callback(songInfo);
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
    songInfo.notesHit = songInfoTemp.songInfoRead.notesHit;
    songInfo.errors = songInfoTemp.songInfoRead.errors;
    songInfo.songPlayCount = songInfoTemp.songInfoRead.songPlayCount;
    songInfo.song = ParseSongName(songInfo.songFilePath, process);
}

ParsedMemoryInfo::Song Sniffer::ParseSongName(std::string& file, PEProcess& process)
{
    ParsedMemoryInfo::Song song{};

    try
    {
        std::string formatted = Util::ReplaceAllString(file, "\\", "/");

        auto splt = Util::SplitString(formatted, '/', true);

        if (splt.size() == 0)
        {
            return song;
        }

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
        if (running && process.StillAlive())
        {
            logger->LogException(e, this, nameof(ParseSongName));
        }
    }

    return song;
}

int Sniffer::GetCurrentInformation(PEProcess& process)
{
    try 
    {
        std::string menuAddress = process.ReadMemoryString(memoryMapping.menu.address, 16);
        if (Util::ValidString(menuAddress))
        {
            MenuType menuType = Util::SafeMapRetrieval(Synthesia::MENU_MAPPING, menuAddress, MenuType::UNKNOWN);

            LPVOID songPausedAddress = process.ReadMemoryAddressChain(memoryMapping.songPaused.address, 
                memoryMapping.songPaused.offsets.data(), memoryMapping.songPaused.offsets.size());

            if (songPausedAddress != 0x0)
            {
                std::string songPausedStr = process.ReadMemoryString(songPausedAddress, 12, nullptr, memoryMapping.songPaused.offsetRun);

                if (songPausedStr == "Song Paused")
                {
                    menuType = MenuType::IN_GAME;
                }
            }

            GameModeFlag gameMode = GameModeFlag::NONE;
            if (menuType == MenuType::GAME_MODE_SELECTION)
            {
                LPVOID gameModeAddress = process.ReadMemoryAddressChain(memoryMapping.gameMode.address, memoryMapping.gameMode.offsets.data(),
                    memoryMapping.gameMode.offsets.size());
                std::string gameModeStr = process.ReadMemoryString(gameModeAddress, 32);
                auto splt = Util::SplitString(gameModeStr, std::string(" • "), true);

                for (size_t i = 0; i < splt.size(); i++) 
                {
                    GameModeFlag flag = Util::SafeMapRetrieval(Synthesia::GAME_MODE_MAPPING, splt[i], GameModeFlag::UNKNOWN);

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

        LPVOID songInfoAddress = process.ReadMemoryAddress(memoryMapping.songInfo.address);
        songInfoTemp.songInfoRead = process.ReadMemoryStruct<MemoryInfoStructs::S1>(songInfoAddress, memoryMapping.songInfo.offsetRun);

        std::string songFilePath = process.ReadMemoryString(songInfoTemp.songInfoRead.addressToSongFilePath, MAX_PATH * 2);
        songInfo.songFilePath = songFilePath;

        LPVOID totalTimeAddress = process.ReadMemoryAddressChain(memoryMapping.totalTime.address, memoryMapping.totalTime.offsets.data(),
            memoryMapping.totalTime.offsets.size());
        std::string totalTime = process.ReadMemoryString(totalTimeAddress, 16, nullptr, memoryMapping.totalTime.offsetRun);

        if (Util::ValidString(totalTime))
        {
            songInfo.timeTotal = ConvertTimeToUInt64(totalTime);
        }

        LPVOID currentTimeAddress = process.ReadMemoryAddressChain(memoryMapping.currentTime.address, memoryMapping.currentTime.offsets.data(),
            memoryMapping.currentTime.offsets.size());
        std::string currentTime = process.ReadMemoryString(currentTimeAddress, 16, nullptr, memoryMapping.currentTime.offsetRun);

        if (Util::ValidString(currentTime))
        {
            songInfo.timeCurrent = ConvertTimeToUInt64(currentTime);
        }
        
        LPVOID notesAddress = process.ReadMemoryAddressChain(memoryMapping.notes.address, memoryMapping.notes.offsets.data(),
            memoryMapping.notes.offsets.size());
        std::string notes = process.ReadMemoryString(notesAddress, 16, nullptr, memoryMapping.notes.offsetRun);

        if (Util::ValidString(notes))
        {
            auto splt = Util::SplitString(notes, '/', true);
            songInfo.notesMax = Util::SafeStringToUInt32(splt.at(1));
        }

    }
    catch (std::exception e)
    {
        if (running && process.StillAlive())
        {
            logger->LogException(e, this, nameof(GetCurrentInformation));
        }

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
        else
        {
            return 0;
        }

        sub = "";
    }

    return total;
}

void Sniffer::CheckVersion(PEProcess& process)
{
    int valid = 0;
    for (Synthesia::MemoryMap& mapping : Synthesia::MEMORY_MAPPING)
    {
        int versionAddress = mapping.version.first;
        int revisionAddress = mapping.revision.first;

        try
        {
            version = process.ReadMemoryString(versionAddress);
            revision = process.ReadMemoryString(revisionAddress);
        }
        catch (std::exception e)
        {
            version = "";
            revision = "";
        }

        const char* ver = mapping.version.second;
        const char* rev = mapping.revision.second;

        if (version == ver && revision == rev)
        {
            valid = 1;
            memoryMapping = mapping;

            songInfo.synthesiaVersionInfo = "Synthesia v" + version + "/" + revision;

            logger->Log(songInfo.synthesiaVersionInfo, this, LogType::LOG_DEBUG);
            break;
        }
    }

    if (!valid)
    {
        if (guiRequestCallback)
        {
            std::function<void()> cancel = []() 
            {
                exit(-1);
            };

            VariableMessageBox obj
            {
                "Sorry, SynthesiaSniffer doesn't support this version of Synthesia. Please make a request or update your Synthesia.",
                std::vector<const char*> { "Exit" },
                std::vector<std::function<void()>> { cancel }
            };

            guiRequestCallback(obj);

            while (true)
            {
                Sleep(1000);
            }
        }
        else
        {
            exit(-1);
        }
    }
}
