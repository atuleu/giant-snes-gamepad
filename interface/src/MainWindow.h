#pragma once

#include "ui_MainWindow.h"

#include <QMainWindow>


namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT
public :
	explicit MainWindow(QWidget * parent = 0);
	~MainWindow();

private :

	Ui::MainWindow * d_ui;

};
