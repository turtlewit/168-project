#pragma once

#include <Godot.hpp>
#include <Node.hpp>

class NetworkSignalManager : public godot::Node {
	GODOT_CLASS(NetworkSignalManager, godot::Node)

public:
	static void _register_methods();
	static NetworkSignalManager* get_singleton() { return singleton; }

	void _init();
	void _on_network_start();
	void _ready();
	void _process(float delta);

	void emit_network_signal(godot::String name, godot::Array args);
	void server_emit_signal(godot::String name, godot::Array args);

	NetworkSignalManager();
	~NetworkSignalManager();

private:
	static NetworkSignalManager* singleton;

	bool is_network_connected = false;
};
