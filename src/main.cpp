#include "./ui/SynthesiaSniffer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SynthesiaSniffer w;
    w.show();
    return a.exec();
}
