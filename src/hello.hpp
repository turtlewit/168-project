#pragma once

#include <Godot.hpp>
#include <Node.hpp>

class Hello : public godot::Node {
	GODOT_CLASS(Hello, godot::Node)

public:
	static void _register_methods();

	void _init();
	void _ready();

	Hello();
	~Hello();

};