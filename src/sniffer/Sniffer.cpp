#include "Sniffer.h"
#include "PEProcess.h"

wchar_t* PROCESS_NAME = _T("Synthesia.exe");

std::vector<std::tuple<const char*, const char*>> VALID_SYNTHESIA_VERSIONS
{
    { "10.6", "r5425" }
};

Sniffer::Sniffer(QMainWindow* window)
{
    this->window = window;
}

void Sniffer::Init()
{
    while (running)
    {
        PEProcess process = PEProcess::WaitForProcessAvailability(PROCESS_NAME, &running);
        process.SetModule(PROCESS_NAME);

        version = process.ReadMemoryString(0x376C30);
        revision = process.ReadMemoryString(0x376C38);

        CheckVersion();

        while (process.StillAlive() && running)
        {
            GetCurrentInformation(process);

            Sleep(100);
        }
    }
}

void Sniffer::GetCurrentInformation(PEProcess process)
{
    std::string songFile = process.ReadMemoryStringFromAddress(0x4201F0, MAX_PATH * 2, TRUE);

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

    if (!valid)
    {
        QMessageBox::StandardButton button = QMessageBox::critical(window, "Error", 
            "Sorry, SynthesiaSniffer doesn't support this version of Synthesia. Please make a request or update your Synthesia.", QMessageBox::Ok);

        exit(-1);
    }
}
