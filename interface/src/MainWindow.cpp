#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "common.h"

#include <libusb.h> 


#include <glog/logging.h> 

#include <QSignalMapper>

#include "CellViewer.h"
#include "ui_CellViewer.h"

GSGParam_e CellByButton[NUM_BUTTONS] = {
	CELL_1,
	CELL_2,
	CELL_3,
	CELL_4,
	CELL_5,
	CELL_6,
	CELL_7,
	CELL_8,
	CELL_9,
	CELL_10,
	CELL_11,
	CELL_12
};


MainWindow::MainWindow(QWidget * parent )
	: QMainWindow(parent)
	, d_ui(new Ui::MainWindow) 
	, d_thresholdMapper(new QSignalMapper(this) )
	, d_releaseMapper(new QSignalMapper(this) ) {
	
	d_ui->setupUi(this);

	LOG(INFO) << "Initializing libsusb";
	lusb_call(libusb_init,NULL);

	d_gamepads = Gamepad::ListAll();
	

	for(Gamepad::List::const_iterator g = d_gamepads.begin();
	    g != d_gamepads.end();
	    ++g ) {
		//TODO add serial number
		d_ui->gamepadSelectBox->addItem(QString::number(g - d_gamepads.begin()),+ ": serial number : N.A.");
	}

	if ( !d_gamepads.empty() ) {
		d_ui->gamepadSelectBox->setCurrentIndex(0);
	}

	d_cellViewers.assign(12,NULL);
	d_cellViewers[UP]            = d_ui->upCell;
	d_cellViewers[DOWN]          = d_ui->downCell;
	d_cellViewers[RIGHT]         = d_ui->rightCell;
	d_cellViewers[LEFT]          = d_ui->leftCell;
	d_cellViewers[A]             = d_ui->aCell;
	d_cellViewers[B]             = d_ui->bCell;
	d_cellViewers[X]             = d_ui->xCell;
	d_cellViewers[Y]             = d_ui->yCell;
	d_cellViewers[START]         = d_ui->startCell;
	d_cellViewers[SELECT]        = d_ui->selectCell;
	d_cellViewers[TRIGGER_RIGHT] = d_ui->rightTriggerCell;
	d_cellViewers[TRIGGER_LEFT]  = d_ui->leftTriggerCell;
	
	d_cellNames.assign(12,NULL);

	d_cellNames[UP]            = tr("Pad Up");
	d_cellNames[DOWN]          = tr("Pad Down");
	d_cellNames[LEFT]          = tr("Pad Left");
	d_cellNames[LEFT]          = tr("Pad Right");
	d_cellNames[A]             = tr("Button A");
	d_cellNames[B]             = tr("Button B");
	d_cellNames[X]             = tr("Button X");
	d_cellNames[Y]             = tr("Button Y");
	d_cellNames[START]         = tr("Button Start");
	d_cellNames[SELECT]        = tr("Button Select");
	d_cellNames[TRIGGER_LEFT]  = tr("Trigger Left");
	d_cellNames[TRIGGER_RIGHT] = tr("Trigger Right");
	

	for( unsigned int i = 0; i < NUM_BUTTONS; ++i) {
		d_cellViewers[i]->d_ui->nameLabel->setText(d_cellNames[i]);
		
		connect(d_cellViewers[i]->d_ui->thresholdBox,SIGNAL(valueChanged(int)),
		        d_thresholdMapper,SLOT(map()));
		d_thresholdMapper->setMapping(d_cellViewers[i]->d_ui->thresholdBox,
		                              CellByButton[i]);

		connect(d_cellViewers[i]->d_ui->releaseBox,SIGNAL(valueChanged(int)),
		        d_thresholdMapper,SLOT(map()));
		
		d_releaseMapper->setMapping(d_cellViewers[i]->d_ui->releaseBox,
		                            CellByButton[i]);
	} 
}


MainWindow::~MainWindow(){
	Close();
	libusb_close(NULL);
}



void MainWindow::on_gamepadSelectBox_currentIndexChanged(int index) {
	Close();
	if(index >= 0) {
		Open(d_gamepads[index]);
	}
}


void MainWindow::Close() {
	if( ! d_usedGamepad ) {
		return;
	}
	
	// TODO close timer


	d_usedGamepad->Close();
	d_usedGamepad = Gamepad::Ptr();
}


void MainWindow::Open(const Gamepad::Ptr & gamepad ) {
	d_usedGamepad = gamepad;
	d_usedGamepad->Open();

	// TODO load all value in viewer
	

	// TODO start timer

}
