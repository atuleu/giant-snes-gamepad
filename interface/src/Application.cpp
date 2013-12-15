#include "Application.h"

#include <iostream> 
#include <unistd.h>

#include <libusb.h> 

#include <glog/logging.h>

#include "common.h"
#include "Gamepad.h"

#include <ncurses.h>

class LUsb : public StaticLibWrapper {
public :
	LUsb() {
		LOG(INFO) << "Initializing libusb context";
		lusb_call(libusb_init,NULL);
	}
	virtual ~LUsb() {
		LOG(INFO) << "Cleaning up libusb";
		libusb_exit(NULL);
	}
};

class NCurses : public StaticLibWrapper {
public :
	NCurses() {
		LOG(INFO) << "Initializing ncurses";
		if(initscr() == NULL ) {
			throw std::runtime_error("Could not initialize ncurses");
		}
	}

	virtual ~NCurses() {
		LOG(INFO) << "Cleaning ncurses";
		if(ERR == endwin() ) {
			LOG(ERROR) << "Could not clean ncurses";
		}
	}
};

Application::Application() 
	: d_lusb(new LUsb())
	, d_ncurses(/*new NCurses()*/) { //disable ncurses for early dev
	LOG(INFO) << "Starting up application.";
}


Application::~Application() {	
	LOG(INFO) << "Cleaning up Application";
}


void Application::Run() {
	Gamepad::List gamepads = Gamepad::ListAll();

	if(gamepads.empty()) {
		throw std::runtime_error("No Gamepad found");
	}

	if(gamepads.size() > 1) {
		throw std::runtime_error("Mechanism for Gamepad selection is not implemented");
	}
	unsigned int selected = 0;
	Gamepad::Ptr dev = gamepads[0];
	gamepads.clear();


	LOG(INFO) << "Opening device " << selected;
	dev->Open();
	libusb_set_debug(NULL,3);
	LOG(INFO) << "Reading LED Frequency";
	dev->SetMaxRetries(1);

	Gamepad::ListOfParameterID ids;
	Gamepad::ListOfParameter params;
	ids.push_back(LED_PERIOD);
	ids.push_back(CELL_1);
	ids.push_back(CELL_2);
	ids.push_back(CELL_3);
	ids.push_back(CELL_4);
	ids.push_back(CELL_5);
	ids.push_back(CELL_6);
	ids.push_back(CELL_7);
	
	dev->GetParameters(ids,params); 

	std::cout << std::endl << "Led Period is "<< params[0].Value << " ms" 
	          << std::endl << std::endl;

}
