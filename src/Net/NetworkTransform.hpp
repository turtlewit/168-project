#pragma once

#include <Godot.hpp>
#include <Node.hpp>

#include "Net/NetworkNode.hpp"

class NetworkTransform : public NetworkNode<godot::Node, NetworkTransform> {
	GODOT_CLASS(NetworkTransform, godot::Node)

public:
	static void _register_methods();

	void sync();

	void _on_network_start() override;
	void _init();
	void _process(float delta);

	NetworkTransform();

private:
	void master_initial_sync();

	void puppet_sync_position(godot::Vector3 pos);
	void puppet_sync_position_child(uint8_t index, godot::Vector3 pos);
	void puppet_sync_rotation(godot::Vector3 euler);
	void puppet_sync_rotation_child(uint8_t index, godot::Vector3 euler);
	void puppet_sync_scale(godot::Vector3 scale);
	void puppet_sync_scale_child(uint8_t index, godot::Vector3 scale);

	godot::Array children_to_sync;
	float sync_interval;
	bool sync_position;
	bool sync_rotation;
	bool sync_scale;

	float current_time;
};
