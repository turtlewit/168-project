#include <SceneTree.hpp>

#include "Net/NetworkIdentity.hpp"
#include "Net/NetworkManager.hpp"

using namespace godot;

void NetworkIdentity::_register_methods()
{
	register_signal<NetworkIdentity>("network_node_start", Dictionary());

	register_property("is_player", &NetworkIdentity::is_player, false);

	register_method("_ready", &NetworkIdentity::_ready);
	register_method("_on_network_start", &NetworkIdentity::_on_network_start);
	register_method("_on_network_peer_connected", &NetworkIdentity::_on_network_peer_connected);
	register_method("_on_network_peer_disconnected", &NetworkIdentity::_on_network_peer_disconnected);
}

void NetworkIdentity::_init()
{
	is_player = false;
}

void NetworkIdentity::_ready()
{
	if (get_tree()->has_network_peer()) {
		// Give time for the rest of the network nodes to connect to the signal.
		call_deferred("_on_network_start");
		return;
	}

	set_process(false);

	NetworkManager::get_singleton()->connect("network_connected", this, "_on_network_start");
	if (get_parent()->has_method("_on_network_connected"))
		NetworkManager::get_singleton()->connect("network_connected", get_parent(), "_on_network_connected");
}

void NetworkIdentity::_on_network_start()
{
	if (is_player && is_network_master()) {
		get_tree()->connect("network_peer_connected", this, "_on_network_peer_connected", Array());
	}
	
	if (!is_player) {
		get_parent()->set_network_master(NetworkManager::SERVER_ID);
	} else {
		get_tree()->connect("network_peer_disconnected", this, "_on_network_peer_disconnected");
	}
	
	if (get_network_master() != 0 && !is_network_master()) {
		Node* parent = get_parent();
		parent->set_process(false);
		parent->set_physics_process(false);
		parent->set_process_input(false);
		parent->set_process_unhandled_input(false);
		parent->set_process_unhandled_key_input(false);
		set_process(false);
	}

	emit_signal("network_node_start");
}

void NetworkIdentity::_on_network_peer_connected(int64_t id)
{
	// Whenever a new client connects, it's the master player's job to make sure
	// a copy is spawned on the connecting client.
	NetworkManager::get_singleton()->rpc_id(id, "spawn_player_with_master", get_tree()->get_network_unique_id());
}

void NetworkIdentity::_on_network_peer_disconnected(int64_t id)
{
	if (id == get_network_master()) {
		get_parent()->queue_free();
	}
}
