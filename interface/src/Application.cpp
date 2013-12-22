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
	LOG(INFO) << "Done";


}
