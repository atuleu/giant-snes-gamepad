#include "Application.h"

#include <iostream> 
#include <unistd.h>

#include <libusb.h> 

#include <glog/logging.h>

Application::Application() {
	LOG(INFO) << "Hello World!";
}


Application::~Application() {
	LOG(INFO) << "Good bye World!";
}


void Application::Run() {
	for (unsigned int seconds = 10; seconds > 0 ; --seconds) {
		std::cout << "I will crash in " << seconds << " seconds" << std::endl;
		sleep(1);
	}
	
	throw std::runtime_error("I crashed, told you so!");
	 
}
