#include "SceneTree.hpp"
#include "MultiplayerAPI.hpp"

#include "NetworkSignalManager.hpp"
#include "NetworkManager.hpp"

using namespace godot;

void NetworkSignalManager::_register_methods()
{
	register_method("_ready", &NetworkSignalManager::_ready);
	register_method("_process", &NetworkSignalManager::_process);
	register_method("_on_network_start", &NetworkSignalManager::_on_network_start);
	register_method("emit_network_signal", &NetworkSignalManager::emit_network_signal);
	register_method("server_emit_signal", &NetworkSignalManager::server_emit_signal, GODOT_METHOD_RPC_MODE_MASTER);

	register_signal<NetworkSignalManager>("player_hit", Dictionary::make(
				String("net_id"), Variant::Type::INT,
				String("damage"), Variant::Type::INT));
}

void NetworkSignalManager::_init()
{
	singleton = this;
}

void NetworkSignalManager::_on_network_start()
{
	set_network_master(NetworkManager::SERVER_ID);
	set_process(true);
	is_network_connected = true;
}

void NetworkSignalManager::_ready()
{
	rpc_config("emit_signal", MultiplayerAPI::RPC_MODE_REMOTE);

	if (get_tree()->has_network_peer()) {
		call_deferred("_on_network_start");
		return;
	}

	set_process(false);
	NetworkManager::get_singleton()->connect("network_connected", this, "_on_network_start", godot::Array());
}

void NetworkSignalManager::_process(float delta)
{
}

void NetworkSignalManager::emit_network_signal(String name, Array args)
{
	if (!is_network_connected)
		return;


	if (get_tree()->get_network_unique_id() == NetworkManager::SERVER_ID) {
		server_emit_signal(name, args);
	} else {
		rpc_id(NetworkManager::SERVER_ID, "server_emit_signal", Array::make(name, args));
	}
}

void NetworkSignalManager::server_emit_signal(godot::String name, godot::Array args)
{
	emit_signal(name, args);
	args.insert(0, name);
	rpc("emit_signal", args);
}

NetworkSignalManager::NetworkSignalManager()
{
}

NetworkSignalManager::~NetworkSignalManager()
{
}

NetworkSignalManager* NetworkSignalManager::singleton = nullptr;
