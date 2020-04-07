#pragma once

#include <Godot.hpp>
#include <Node.hpp>
#include <Array.hpp>

class NetworkSync : public godot::Node {
	GODOT_CLASS(NetworkSync, godot::Node)

public:
	static void _register_methods();

	void _init();
	void _ready();
	void _on_network_start();
	void _process();

	NetworkSync();

private:
	godot::Array sync_properties;
	float sync_interval;
	godot::Array delete_on_not_authority;
};
