#include "PauseMenu.hpp"

#include "Utils/Defs.hpp"

#include <Input.hpp>
#include <SceneTree.hpp>

using namespace godot;

namespace {
	Input* inp;
}

void PauseMenu::_register_methods()
{
	REGISTER_METHOD(PauseMenu, _ready);
	REGISTER_METHOD(PauseMenu, _process);
	
	REGISTER_METHOD(PauseMenu, resume);
	REGISTER_METHOD(PauseMenu, quit);

	register_signal<PauseMenu>("menu_closed", Dictionary());
}

void PauseMenu::_init()
{
}

void PauseMenu::_ready()
{
	inp = Input::get_singleton();
}

void PauseMenu::_process(float delta)
{
	if (inp->is_action_just_pressed("sys_quit"))
		resume();
}

PauseMenu::PauseMenu()
{
}

PauseMenu::~PauseMenu()
{
}


void PauseMenu::resume()
{
	emit_signal("menu_closed");
	queue_free();
}


void PauseMenu::quit()
{
	get_tree()->quit();
}
