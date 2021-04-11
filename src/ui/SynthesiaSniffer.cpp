#include "SynthesiaSniffer.h"

SynthesiaSniffer::SynthesiaSniffer(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    sniffer = new Sniffer(this);
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
