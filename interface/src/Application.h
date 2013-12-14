#pragma once

#include <memory>

class Application {
public :
	typedef std::shared_ptr<Application> Ptr;

	Application();
	~Application();
	
	void Run();

private :

};

