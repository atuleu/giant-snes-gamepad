#include "Application.h"

#include <iostream>
#include <exception>
#include <signal.h>

Application::Ptr app;

void SigHandler(int c) {
	// kills the application, if needed
	std::cout << "Interrupted, I should crash garcefully !" << std::endl;
	app = Application::Ptr();
	exit(0);
}

int main() {
	int res = 0;
	try {
		signal(SIGINT,&SigHandler);
		app = Application::Ptr(new Application());
		app->Run();
	} catch (const std::exception & e) {
		std::cerr << "Got error : "<< e.what() << std::endl;
	}
	return res;
}
