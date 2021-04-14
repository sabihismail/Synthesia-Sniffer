#include "SynthesiaSniffer.h"

SynthesiaSniffer::SynthesiaSniffer(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    settings = new Settings();
    discord = new DiscordRPC();
    sniffer = new Sniffer(settings);

    sniffer->OnUpdate([&](ParsedSongInfo& songInfo)
        {
            discord->SetActivity(songInfo);
        });

    sniffer->OnGUIRequest([&](VariableMessageBox& obj)
        {
            QMetaObject::invokeMethod(this, [this, obj]
                {
                    QMessageBox msgBox(this);
                    msgBox.setText(tr(obj.msg));

                    for (int i = 0; i < obj.options.size(); i++)
                    {
                        msgBox.addButton(tr(obj.options.at(i)), QMessageBox::YesRole);
                    }

                    msgBox.exec();

                    QAbstractButton* clicked = msgBox.clickedButton();
                    int index = msgBox.buttons().indexOf(clicked);

                    obj.callbacks.at(index)();
                }, Qt::QueuedConnection);
        });

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
