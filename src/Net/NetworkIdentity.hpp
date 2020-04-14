#pragma once

#include <Godot.hpp>
#include <Node.hpp>

// NetworkIdentity
//
// Signals
//   network_node_start()
class NetworkIdentity : public godot::Node {
	GODOT_CLASS(NetworkIdentity, godot::Node)

public:
	static void _register_methods();

	void _ready();
	void _on_network_start();
	void _on_network_peer_connected(int64_t id);

	void _init();

private:
	bool is_player;

};
