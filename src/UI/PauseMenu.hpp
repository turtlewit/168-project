#pragma once

#include <Godot.hpp>
#include <Control.hpp>

namespace godot {

class PauseMenu : public Control {
	GODOT_CLASS(PauseMenu, Control)

public:
	static void _register_methods();

	void _init();
	void _ready();
	void _process(float delta);

	PauseMenu();
	~PauseMenu();

private:
	void resume();
	void quit();
};

}


