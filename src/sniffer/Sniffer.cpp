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

Sniffer::Sniffer(Settings* settings)
{
    this->settings = settings;
}

void Sniffer::Init()
{
    while (running)
    {
        PEProcess process = PEProcess::WaitForProcessAvailability(PROCESS_NAME, &running);
        process.SetModule(PROCESS_NAME);

        for (std::tuple<int, int> address : SYNTHESIA_VERSION_ADDRESSES)
        {
            int versionAddress = std::get<0>(address);
            int revisionAddress = std::get<1>(address);

            version = process.ReadMemoryString(versionAddress);
            revision = process.ReadMemoryString(revisionAddress);
        }

        CheckVersion();

        while (synthesiaVersion.version.empty())
        {
            Sleep(100);
        }

        while (process.StillAlive() && running)
        {
            int success = GetCurrentInformation(process);

            if (success)
            {
                FormatSongInfo(process);

                for (std::function<void(ParsedSongInfo&)> callback : callbacks)
                {
                    callback(songInfo);
                }
            }

            Sleep(100);
        }
    }
}

void Sniffer::OnUpdate(std::function<void(ParsedSongInfo&)> callback)
{
    callbacks.push_back(callback);
}

void Sniffer::OnGUIRequest(std::function<void(VariableMessageBox&)> callback)
{
    guiRequestCallback = callback;
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

ParsedSongInfo::Song Sniffer::ParseSongName(std::string& file)
{
    std::string formatted = Util::ReplaceAllString(file, "\\", "/");

    auto splt = Util::SplitString(formatted, '/', TRUE);
    std::string filename = splt.at(splt.size() - 1);

    auto mapping = Util::ParseStringByFormat(filename, settings->songFormat);

    ParsedSongInfo::Song song{};

    auto title = Util::SafeMapRetrieval(mapping, std::string("title"), std::string(""));
    auto artist = Util::SafeMapRetrieval(mapping, std::string("artist"), std::string(""));

    Util::ReplaceAllStringInPlace(artist, ".mid", "");
    Util::ReplaceAllStringInPlace(artist, ".midi", "");

    Util::ReplaceAllStringInPlace(title, ".mid", "");
    Util::ReplaceAllStringInPlace(title, ".midi", "");

    song.title = title;
    song.artist = artist;

    return song;
}

int Sniffer::GetCurrentInformation(PEProcess& process)
{
    //std::string songFile = process.ReadMemoryStringFromAddress(0x004201F0, MAX_PATH * 2, TRUE);

    //int midiOffsets[] = { 0x0, 0x88, 0x0, 0x1C };
    //LPVOID midiAddress = process.ReadMemoryAddressChain(0x0041EE34, midiOffsets, sizeof(midiOffsets));

    LPVOID songInfoAddress = process.ReadMemoryAddress(0x0041EE34);

    songInfoTemp.songInfoRead = process.ReadMemoryStruct<MemorySongInfo::MemorySongInfoS1>(songInfoAddress);

    int totalTimeOffsets[] = { 0xA4, 0x0, 0xC4, 0x1B0, 0x0, 0x180 };
    LPVOID totalTimeAddress = process.ReadMemoryAddressChain(0x0041EE38, totalTimeOffsets, sizeof(totalTimeOffsets));
    std::string totalTime = process.ReadMemoryString(totalTimeAddress, 16, 364);

    if (Util::ValidString(totalTime))
    {
        songInfo.timeTotal = ConvertTimeToUInt64(totalTime);
    }

    int currentTimeOffsets[] = { 0xA4, 0x0, 0xD0, 0x168, 0x1CC, 0x84 };
    LPVOID currentTimeAddress = process.ReadMemoryAddressChain(0x0041EE38, currentTimeOffsets, sizeof(currentTimeOffsets));
    std::string currentTime = process.ReadMemoryString(currentTimeAddress, 16, 364);

    if (Util::ValidString(currentTime))
    {
        songInfo.timeCurrent = ConvertTimeToUInt64(currentTime);
    }

    /*int errorsOffsets[] = { 0xA4, 0x0, 0x10, 0x11C, 0x8, 0x184 };
    LPVOID errorsAddress = process.ReadMemoryAddressChain(0x0041EE38, errorsOffsets, sizeof(errorsOffsets));
    std::string errors = process.ReadMemoryString(errorsAddress, 16, 364);

    if (Util::ValidString(errors))
    {
        songInfo.errors = Util::SafeStringToInt(errors.substr(1));
    }*/

    int notesOffsets[] = { 0xA4, 0x0, 0xC4, 0x124, 0x19C, 0x180 };
    LPVOID notesAddress = process.ReadMemoryAddressChain(0x0041EE38, notesOffsets, sizeof(notesOffsets));
    std::string notes = process.ReadMemoryString(notesAddress, 16, 364);

    if (Util::ValidString(notes))
    {
        auto splt = Util::SplitString(notes, '/', TRUE);
        songInfo.notesMax = Util::SafeStringToUInt32(splt.at(1));
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
