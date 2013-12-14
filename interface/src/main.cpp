#include "Application.h"

#include <iostream>
#include <exception>
#include <signal.h>

#include <glog/logging.h>


Application::Ptr app;

void SigHandler(int c) {
	// kills the application, if needed
	LOG(INFO) << "Interrupted by signal " << c ;
	app = Application::Ptr();
	exit(0);
}

int main(int /*argc*/, char ** argv) {
	int res = 0;
	try {
		FLAGS_colorlogtostderr = true;
		// Initialize Google's logging library.
		google::InitGoogleLogging(argv[0]);

		signal(SIGINT,&SigHandler);
		app = Application::Ptr(new Application());
		app->Run();
	} catch (const std::exception & e) {
		app = Application::Ptr();
		LOG(ERROR) << "Got error : "<< e.what();
	}
	return res;
}
