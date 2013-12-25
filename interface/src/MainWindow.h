#pragma once

#include "ui_MainWindow.h"

#include <QMainWindow>


#include "Gamepad.h"

namespace Ui {
	class MainWindow;
}

class CellViewer;
class QSignalMapper;

class MainWindow : public QMainWindow {
	Q_OBJECT
public :
	explicit MainWindow(QWidget * parent = 0);
	~MainWindow();


public slots :
	void on_gamepadSelectBox_currentIndexChanged(int index);

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

};
