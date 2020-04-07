#pragma once

#include <Godot.hpp>
#include <Node.hpp>

enum class NetworkStartMode : int64_t {
	HOST,
	SERVER,
	CLIENT,
};


// NetworkStarter
//
// Properties:
//   start_mode: NetworkStartMode
//     Determines how to initialize the network.
//     HOST: server and player.
//     SERVER: start the server.
//     CLIENT: connect to server at given address and port.
//
//   autostart: bool
//     initializes the network in _ready if true;
class NetworkStarter : public godot::Node {
	GODOT_CLASS(NetworkStarter, godot::Node)

public:
	static void _register_methods();

	void start_network();

	void _init();
	void _ready();

	NetworkStarter();

private:
	void set_start_mode(int64_t start_mode) { this->start_mode = static_cast<NetworkStartMode>(start_mode); }
	int64_t get_start_mode() const { return static_cast<int64_t>(start_mode); }

	bool autostart;
	NetworkStartMode start_mode;
};
