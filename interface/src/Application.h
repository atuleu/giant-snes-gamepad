#pragma once

#include <memory>



class StaticLibWrapper {
public :
	typedef std::shared_ptr<StaticLibWrapper> Ptr;
	virtual ~StaticLibWrapper() {};
};


class Application {
public :
	typedef std::shared_ptr<Application> Ptr;

	Application();
	~Application();
	
	void Run();


private :
	//small pattern to call static cleaning libraries

	StaticLibWrapper::Ptr d_lusb, d_ncurses;

};

