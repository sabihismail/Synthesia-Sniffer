#include "SynthesiaSniffer.h"

SynthesiaSniffer::SynthesiaSniffer(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    discord = new DiscordRPC();
    sniffer = new Sniffer();

    sniffer->OnUpdate([](SongInfo* songInfo)
        {

        });

    snifferThread = std::thread(&SynthesiaSniffer::StartSniffer, this);
}

void SynthesiaSniffer::closeEvent(QCloseEvent *event)
{
    sniffer->running = 0;

    snifferThread.detach();
}

void SynthesiaSniffer::StartSniffer()
{
    sniffer->Init();
}
