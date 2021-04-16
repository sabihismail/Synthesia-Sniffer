#include "SynthesiaSniffer.h"

SynthesiaSniffer::SynthesiaSniffer(QWidget* parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    txtInfoLog = this->findChild<QTextEdit*>("txtInfoLog");
    txtErrorLog = this->findChild<QTextEdit*>("txtErrorLog");
    lblCurrentInfo = this->findChild<QLabel*>("lblCurrentInfo");

    logger = new Logger();
    settings = new Settings();
    discord = new DiscordRPC(logger);
    sniffer = new Sniffer(settings, logger);

    logger->OnLog([this](LogType logType, std::string& str)
        {
            if (logType == LogType::LOG_DEBUG)
            {
                return;
            }

            QString qStr(str.c_str());
            qStr += "\n";

            QMetaObject::invokeMethod(this, [this, logType, qStr]
                {
                    QTextEdit* txt = txtInfoLog;

                    if (logType == LogType::LOG_ERROR)
                    {
                        txt = txtErrorLog;
                    }

                    txt->moveCursor(QTextCursor::End);
                    txt->insertPlainText(qStr);
                    txt->moveCursor(QTextCursor::End);
                }, Qt::QueuedConnection);
        });

    sniffer->OnUpdate([this](ParsedMemoryInfo& memoryInfo)
        {
            discord->SetActivity(memoryInfo);

            QString qStr(memoryInfo.ToString().c_str());

            QMetaObject::invokeMethod(this, [this, qStr]
                {
                    lblCurrentInfo->setText(qStr);
                });
        });

    sniffer->OnGUIRequest([this](VariableMessageBox& obj)
        {
            QMetaObject::invokeMethod(this, [this, &obj]
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
    try
    {
        snifferThread.detach();

        delete sniffer;
        delete discord;
        delete settings;
        delete logger;
    }
    catch (std::exception e)
    {

    }
}

void SynthesiaSniffer::StartSniffer()
{
    try
    {
        sniffer->Init();
    }
    catch (std::exception e)
    {
        logger->LogException(e, this, nameof(StartSniffer));
    }
}
