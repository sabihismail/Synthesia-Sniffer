#pragma once

#include <QtWidgets/QMainWindow>
#include <functional>
#include <iostream>
#include "ui_SynthesiaSniffer.h"
#include "../sniffer/Sniffer.h"
#include "../discord/DiscordRPC.h"

class SynthesiaSniffer : public QMainWindow
{
    Q_OBJECT

public:
    SynthesiaSniffer(QWidget *parent = Q_NULLPTR);

private:
    Ui::SynthesiaSnifferClass ui;
    Sniffer* sniffer = nullptr;
    DiscordRPC* discord = nullptr;
    std::thread snifferThread;

    void closeEvent(QCloseEvent* bar);
    void SynthesiaSniffer::StartSniffer();
};
