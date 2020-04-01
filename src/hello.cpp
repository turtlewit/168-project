#include "hello.hpp"

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
}

Hello::Hello()
{
}

Hello::~Hello()
{
}