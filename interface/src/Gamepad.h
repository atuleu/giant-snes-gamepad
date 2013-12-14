#pragma once

#include <vector>
#include <memory>

class Gamepad {
public :
	typedef std::shared_ptr<Gamepad> Ptr;
	typedef std::vector<Gamepad::Ptr> List;
	
	static List ListAllGamepads();

	~Gamepad();

	void Open();
	void Close();

private :
	Gamepad();
};
