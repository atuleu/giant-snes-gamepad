#pragma once

#include <QWidget>

namespace Ui {
	class CellViewer;
}

class CellViewer : public QWidget {
Q_OBJECT
public :
	CellViewer(QWidget * parent = NULL );
	~CellViewer();

private:
	friend class MainWindow;
	Ui::CellViewer * d_ui;
};
