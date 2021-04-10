#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SynthesiaSniffer.h"

class SynthesiaSniffer : public QMainWindow
{
    Q_OBJECT

public:
    SynthesiaSniffer(QWidget *parent = Q_NULLPTR);

private:
    Ui::SynthesiaSnifferClass ui;
};
