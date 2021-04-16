#pragma once

#include "ui_SynthesiaSniffer.h"
#include "../util/Settings.h"
#include "../util/Logger.h"
#include "../sniffer/Sniffer.h"
#include "../discord/DiscordRPC.h"
#include "../util/VariableMessageBox.h"
#include <functional>
#include <iostream>
#include <QMainWindow>
#include <QTranslator>
#include <qtextedit.h>

class SynthesiaSniffer : public QMainWindow
{
    Q_OBJECT

public:
    std::shared_ptr<Settings> settings = nullptr;
    std::shared_ptr<Logger> logger = nullptr;
    std::unique_ptr<Sniffer> sniffer = nullptr;
    std::unique_ptr<DiscordRPC> discord = nullptr;

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
