#include <SceneTree.hpp>

#include "Net/NetworkSync.hpp"
#include "Net/NetworkManager.hpp"
#include "Utils/Defs.hpp"

using namespace godot;

void NetworkSync::_register_methods()
{
	NetworkNode<Node, NetworkSync>::_register_methods();

	ADD_INSPECTOR_GROUP(NetworkSync, "Sync", "sync_");
	register_property(
		"sync_properties", 
		&NetworkSync::sync_properties, 
		Array(),
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_TYPE_STRING,
		GODOT_HINT_STRING_ARRAY_OF_STRINGS);
	register_property("sync_interval", 
		&NetworkSync::sync_interval, 
		0.3f,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_RANGE,
		"0.0,50.0");
	register_property("delete_on_not_authority", 
		&NetworkSync::delete_on_not_authority, 
		Array(),
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_TYPE_STRING,
		GODOT_HINT_STRING_ARRAY_OF_NODE_PATHS);

	register_method("master_initial_sync", &NetworkSync::master_initial_sync, GODOT_METHOD_RPC_MODE_MASTER);
	register_method("sync", &NetworkSync::sync);
	register_method("_on_network_start", &NetworkSync::_on_network_start);
	register_method("_process", &NetworkSync::_process);
}

void NetworkSync::master_initial_sync()
{
	for (int i = 0; i < sync_properties.size(); ++i) {
		const String property = sync_properties[i];
		get_parent()->rset_id(get_tree()->get_rpc_sender_id(), property, get_parent()->get(property));
	}
}

void NetworkSync::sync()
{
	for (int i = 0; i < sync_properties.size(); ++i) {
		const String property = sync_properties[i];
		Variant prop = get_parent()->get(property);
		if (!property_cache.has(property)) {
			property_cache[property] = prop;
			get_parent()->rset(property, prop);
			continue;
		}
		if (property_cache[property] != prop) {
			property_cache[property] = prop;
			get_parent()->rset(property, prop);
		}
	}
}

void NetworkSync::_init()
{
}

void NetworkSync::_on_network_start()
{
	set_property_sync_mode(GODOT_METHOD_RPC_MODE_PUPPET);

	if (!is_master()) {

		for (int i = 0; i < delete_on_not_authority.size(); ++i) {
			Godot::print("Deleting...");
			Node* node = get_node(delete_on_not_authority[i]);
			node->queue_free();
		}

		rpc_id(get_network_master(), "master_initial_sync");
	} else {
		set_process(true);
	}
}

void NetworkSync::_process(float delta)
{
	current_time += delta;
	if (current_time >= sync_interval) {
		sync();
		current_time -= sync_interval;
	}
}

NetworkSync::NetworkSync()
	: sync_properties{}, sync_interval{ 0.3f }, delete_on_not_authority{}, current_time{ 0 }
{
}

void NetworkSync::set_property_sync_mode(godot_method_rpc_mode mode)
{
	for (int i = 0; i < sync_properties.size(); ++i) {
		const String property = sync_properties[i];
		get_parent()->rset_config(property, mode);
	}
}
