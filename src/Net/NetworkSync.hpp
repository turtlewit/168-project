#pragma once

#include <Godot.hpp>
#include <Node.hpp>
#include <Array.hpp>

#include "Net/NetworkNode.hpp"

class NetworkSync : public NetworkNode<godot::Node, NetworkSync> {
	GODOT_CLASS(NetworkSync, godot::Node)

public:
	static void _register_methods();

	void master_initial_sync();
	void sync();

	void _init();
	void _on_network_start() override;
	void _process(float delta);

	NetworkSync();

private:
	void set_property_sync_mode(godot_method_rpc_mode mode);

	godot::Array sync_properties;
	godot::Dictionary property_cache;
	float sync_interval;
	godot::Array delete_on_not_authority;
	float current_time;
};
