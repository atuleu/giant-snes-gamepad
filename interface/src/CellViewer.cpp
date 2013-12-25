#include "CellViewer.h"
#include "ui_CellViewer.h"



CellViewer::CellViewer(QWidget * parent)
	: QWidget(parent) 
	, d_ui(new Ui::CellViewer) {

	d_ui->setupUi(this);
}



CellViewer::~CellViewer() {
}
