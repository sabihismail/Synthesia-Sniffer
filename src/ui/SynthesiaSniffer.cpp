#include "SynthesiaSniffer.h"
#include "../sniffer/Sniffer.h"

SynthesiaSniffer::SynthesiaSniffer(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    Sniffer sniffer(this);
}
