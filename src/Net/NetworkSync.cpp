#include "Net/NetworkSync.hpp"
#include "Utils/Defs.hpp"

using namespace godot;

void NetworkSync::_register_methods()
{
	ADD_INSPECTOR_GROUP(NetworkSync, "Sync", "sync_");
	register_property(
		"sync_properties", 
		&NetworkSync::sync_properties, 
		Array(),
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_TYPE_STRING,
		GODOT_HINT_STRING_ARRAY_OF_STRINGS);
	register_property("sync_interval", &NetworkSync::sync_interval, 0.0f);
	register_property("delete_on_not_authority", &NetworkSync::delete_on_not_authority, Array());

	register_method("_ready", &NetworkSync::_ready);
	register_method("_on_network_start", &NetworkSync::_on_network_start);
	register_method("_process", &NetworkSync::_process);
}

void NetworkSync::_init()
{
}

void NetworkSync::_ready()
{
}

void NetworkSync::_on_network_start()
{
}

void NetworkSync::_process()
{
}

NetworkSync::NetworkSync()
	: sync_properties{}, sync_interval{ 0 }, delete_on_not_authority{}
{
}
