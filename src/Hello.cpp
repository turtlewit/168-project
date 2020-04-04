#include "Hello.hpp"

#include "Utils/Mathf.hpp"

using namespace godot;

void Hello::_register_methods()
{
	register_method("_ready", &Hello::_ready);
}

void Hello::_init()
{
}

void Hello::_ready()
{
	Godot::print("Hello!");
	Godot::print(Mathf::choose("Hello 1", "Hello 2", "Hello 3"));
	Godot::print(Mathf::choose(1, 2, 3));
}

Hello::Hello()
{
}

Hello::~Hello()
{
}
