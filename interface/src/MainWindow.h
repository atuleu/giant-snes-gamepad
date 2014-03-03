#pragma once

#include "ui_MainWindow.h"

#include <QMainWindow>


#include "Gamepad.h"

namespace Ui {
	class MainWindow;
}

class CellViewer;
class QSignalMapper;
class QTimer;

class MainWindow : public QMainWindow {
	Q_OBJECT
public :
	explicit MainWindow(QWidget * parent = 0);
	~MainWindow();


public slots :
	void on_gamepadSelectBox_activated(int index);
	void on_thresholdMapper_mapped(int);
	void on_releaseMapper_mapped(int);
	void on_timer_timeout();
	void on_actionQuit_triggered();
	void on_actionSave_In_EEPROM_triggered();
	void on_scanTimer_timeout();
private :

	void Close();
	void Open(const Gamepad::Ptr & gamepad);
	

	Ui::MainWindow * d_ui;
	Gamepad::Ptr     d_usedGamepad;
	Gamepad::List    d_gamepads;

	std::vector<CellViewer*>  d_cellViewers;
	std::vector<QString>      d_cellNames;
	QSignalMapper *           d_thresholdMapper;
	QSignalMapper *           d_releaseMapper;
	QTimer *                  d_timer,* d_scanTimer;
};
