#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SynthesiaSniffer.h"
#include "../sniffer/Sniffer.h"

class SynthesiaSniffer : public QMainWindow
{
    Q_OBJECT

public:
    SynthesiaSniffer(QWidget *parent = Q_NULLPTR);

private:
    Ui::SynthesiaSnifferClass ui;
    Sniffer* sniffer = nullptr;
    std::thread snifferThread;

    void closeEvent(QCloseEvent* bar);
    void SynthesiaSniffer::StartSniffer();
};
