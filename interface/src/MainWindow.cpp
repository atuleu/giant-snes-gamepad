#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "common.h"

#include <libusb.h> 


#include <glog/logging.h> 

#include <QSignalMapper>
#include <QTimer>


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
	, d_releaseMapper(new QSignalMapper(this) ) 
	, d_timer ( new QTimer(this)) 
	, d_scanTimer(new QTimer(this)) {

	d_thresholdMapper->setObjectName("thresholdMapper");
	d_releaseMapper->setObjectName("releaseMapper");
	d_timer->setObjectName("timer");
	d_scanTimer->setObjectName("scanTimer");

	d_ui->setupUi(this);

	LOG(INFO) << "Initializing libsusb";
	lusb_call(libusb_init,NULL);

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
		
		d_thresholdMapper->setMapping(d_cellViewers[i]->d_ui->thresholdBox,
		                              i);

		d_releaseMapper->setMapping(d_cellViewers[i]->d_ui->releaseBox,
		                            i);

		d_cellViewers[i]->setEnabled(false);
	} 

	
	d_ui->saveEEPROMButton->setDefaultAction(d_ui->actionSave_In_EEPROM);
	d_ui->actionSave_In_EEPROM->setEnabled(false);

	on_scanTimer_timeout();
	d_scanTimer->start(500);
}


MainWindow::~MainWindow(){
	Close();
	libusb_close(NULL);
}



void MainWindow::on_gamepadSelectBox_activated(int index) {
	Close();
	if(index >= 0) {
		Open(d_gamepads[index]);
	}
}


void MainWindow::on_timer_timeout() {
	if(!d_usedGamepad) {
		return;
	}
	static uint16_t id = 0;

	Gamepad::LoadCellValues values;

	try {
		d_usedGamepad->FetchLoadCellValues(values);
	} catch ( const LibUsbError & e ) {
		d_ui->statusbar->showMessage(QString("Failed to fetch load cell values: ") + e.what() );
		return;
	}

	if ( values.size() != NUM_BUTTONS ) {
		d_ui->statusbar->showMessage("Could not fetch values, got " + QString::number(values.size()) 
		                             + ", but expected " + QString::number(NUM_BUTTONS) );
		return;
	}

	for(unsigned int i = 0; i < NUM_BUTTONS; ++i ) {
		d_cellViewers[i]->d_ui->valueDisplayLabel->setText(QString::number(values[i] & 0x3ff));
	}

}

void MainWindow::on_thresholdMapper_mapped(int button) {
	if ( ! d_usedGamepad ) {
		return;
	}
	try {
	d_usedGamepad->SetParam((GSGParam_e)(CellByButton[button] + CELL_THRESHOLD), 
	                        d_cellViewers[button]->d_ui->thresholdBox->value());
	} catch ( const LibUsbError & e ) {
		d_ui->statusbar->showMessage(QString("Got error: ") + e.what());
	}
	
}


void MainWindow::on_releaseMapper_mapped(int button) {
	if ( ! d_usedGamepad ) {
		return;
	}
	try {
		d_usedGamepad->SetParam((GSGParam_e)(CellByButton[button] + CELL_RELEASE), 
		                        d_cellViewers[button]->d_ui->releaseBox->value());
	} catch ( const LibUsbError & e ) {
		d_ui->statusbar->showMessage(QString("Got error: ") + e.what());
	}

}


void MainWindow::on_actionQuit_triggered() {
	Close();
	this->close();
}

void MainWindow::on_actionSave_In_EEPROM_triggered() {
	if(!d_usedGamepad) {
		LOG(ERROR) << "Could not save in EEPROM : no gamepad opened!";
		return;
	}

	d_usedGamepad->SaveParamInEEPROM();
}



void MainWindow::on_scanTimer_timeout() {
	d_gamepads = Gamepad::ListAll();
	
	d_ui->gamepadSelectBox->clear();
	bool openedFound(false);
	for(Gamepad::List::const_iterator g = d_gamepads.begin();
	    g != d_gamepads.end();
	    ++g ) {
		//TODO add serial number
		d_ui->gamepadSelectBox->addItem(QString::number(g - d_gamepads.begin()),+ ": serial number : N.A.");

		if(*g == d_usedGamepad) {
			openedFound = true;
			d_ui->gamepadSelectBox->setCurrentIndex(g - d_gamepads.begin());
		}
	}

	if ( d_usedGamepad && !openedFound) {
		Close();
	}


	// last before initialization
	if ( !d_gamepads.empty() && !d_usedGamepad) {
		d_ui->gamepadSelectBox->setCurrentIndex(0);
		on_gamepadSelectBox_activated(0);
	}

}

void MainWindow::Close() {
	if( ! d_usedGamepad ) {
		return;
	}
	d_ui->actionSave_In_EEPROM->setEnabled(false);


	for( unsigned int i = 0; i < NUM_BUTTONS; ++i ) {
		d_cellViewers[i]->setEnabled(false);
	}

	d_timer->stop();

	d_usedGamepad->Close();
	d_usedGamepad = Gamepad::Ptr();
}


void MainWindow::Open(const Gamepad::Ptr & gamepad ) {
	d_usedGamepad = gamepad;
	d_usedGamepad->Open();


	for( unsigned int i = 0; i < NUM_BUTTONS; ++i ) {
		disconnect(d_cellViewers[i]->d_ui->thresholdBox,SIGNAL(valueChanged(int)),
		           d_thresholdMapper,SLOT(map()));
		disconnect(d_cellViewers[i]->d_ui->releaseBox,SIGNAL(valueChanged(int)),
		           d_releaseMapper,SLOT(map()));
		d_cellViewers[i]->d_ui->thresholdBox->setValue(d_usedGamepad->GetParam((GSGParam_e)(CellByButton[i] + CELL_THRESHOLD)));
		d_cellViewers[i]->d_ui->releaseBox->setValue(d_usedGamepad->GetParam((GSGParam_e)(CellByButton[i] + CELL_RELEASE)));
		connect(d_cellViewers[i]->d_ui->thresholdBox,SIGNAL(valueChanged(int)),
		        d_thresholdMapper,SLOT(map()));
		connect(d_cellViewers[i]->d_ui->releaseBox,SIGNAL(valueChanged(int)),
		        d_releaseMapper,SLOT(map()));

		d_cellViewers[i]->setEnabled(true);
	}


	d_timer->start(100);
	d_ui->actionSave_In_EEPROM->setEnabled(true);
}
