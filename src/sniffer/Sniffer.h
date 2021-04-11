#pragma once

#include "PEProcess.h"
#include <QMessageBox>
#include <QMainWindow>
#include <thread>

class Sniffer
{
public:
	int running = 1;

	Sniffer(QMainWindow* window);
	void Sniffer::Init();

private:
	QMainWindow* window;

	std::string version;
	std::string revision;

	void Sniffer::CheckVersion();
	void Sniffer::GetCurrentInformation(PEProcess process);
};

