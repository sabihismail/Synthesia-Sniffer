#pragma once

#include <QMessageBox>
#include <QMainWindow>

class Sniffer
{
public:
	Sniffer(QMainWindow* window);

private:
	QMainWindow* window;

	std::string version;
	std::string revision;

	void Sniffer::CheckVersion();
};

