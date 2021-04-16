#pragma once

#include "ui_SynthesiaSniffer.h"
#include "../util/Settings.h"
#include "../util/Logger.h"
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
    Settings* settings = nullptr;
    Sniffer* sniffer = nullptr;
    DiscordRPC* discord = nullptr;
    Logger* logger = nullptr;

    SynthesiaSniffer(QWidget *parent = Q_NULLPTR);

private:
    Ui::SynthesiaSnifferClass ui;
    QLabel* lblCurrentInfo = nullptr;
    QTextEdit* txtInfoLog = nullptr;
    QTextEdit* txtErrorLog = nullptr;
    std::thread snifferThread;

    void closeEvent(QCloseEvent* bar);
    void SynthesiaSniffer::StartSniffer();
};
