#include "./ui/SynthesiaSniffer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    SynthesiaSniffer window;
    window.show();
    return app.exec();
}
