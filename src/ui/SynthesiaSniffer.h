#pragma once

#include "ui_SynthesiaSniffer.h"
#include "../util/Settings.h"
#include "../sniffer/Sniffer.h"
#include "../discord/DiscordRPC.h"
#include "../util/VariableMessageBox.h"
#include <functional>
#include <iostream>
#include <QtWidgets/QMainWindow>
#include <QTranslator>

class SynthesiaSniffer : public QMainWindow
{
    Q_OBJECT

public:
    SynthesiaSniffer(QWidget *parent = Q_NULLPTR);
    Settings* settings = nullptr;
    Sniffer* sniffer = nullptr;
    DiscordRPC* discord = nullptr;

private:
    Ui::SynthesiaSnifferClass ui;
    std::thread snifferThread;

    void closeEvent(QCloseEvent* bar);
    void SynthesiaSniffer::StartSniffer();
};
