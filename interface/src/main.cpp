#include "MainWindow.h"

#include <iostream>
#include <exception>


#include <glog/logging.h>



int main(int argc, char ** argv) {
	FLAGS_logtostderr = true;
	FLAGS_colorlogtostderr = true;
	// Initialize Google's logging library.
	google::InitGoogleLogging(argv[0]);


	QApplication application(argc,argv);

	MainWindow main;
	main.show();

	return application.exec();
}
