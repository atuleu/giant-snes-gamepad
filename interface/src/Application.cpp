#include "Application.h"

#include <iostream> 
#include <unistd.h>

Application::Application() {
	std::cout << "Hello World!" << std::endl;
}


Application::~Application() {
	std::cout << "Good bye World!" << std::endl;
}


void Application::Run() {
	for (unsigned int seconds = 10; seconds > 0 ; --seconds) {
		std::cout << "I will crash in " << seconds << " seconds" << std::endl;
		sleep(1);
	}
	
	throw std::runtime_error("I crashed, told you so!");
	 
}
