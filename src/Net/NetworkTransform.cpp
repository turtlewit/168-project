#include <Spatial.hpp>

#include "Net/NetworkTransform.hpp"
#include "Utils/Defs.hpp"

using namespace godot;

void NetworkTransform::_register_methods()
{
	NetworkNode<godot::Node, NetworkTransform>::_register_methods();


	REGISTER_PROPERTY_HINT(
		NetworkTransform,
		Array,
		children_to_sync,
		Array(),
		GODOT_PROPERTY_HINT_TYPE_STRING,
		GODOT_HINT_STRING_ARRAY_OF_NODE_PATHS
	);

	ADD_INSPECTOR_GROUP(NetworkTransform, "Sync", "sync_");
	REGISTER_PROPERTY_HINT(
		NetworkTransform,
		float,
		sync_interval,
		0.033f,
		GODOT_PROPERTY_HINT_RANGE,
		"0.0,50.0"
	);

	register_property("sync_position", &NetworkTransform::sync_position, true);
	register_property("sync_rotation", &NetworkTransform::sync_rotation, true);
	register_property("sync_scale", &NetworkTransform::sync_scale, true);

	register_method("master_initial_sync", &NetworkTransform::master_initial_sync, GODOT_METHOD_RPC_MODE_MASTER);
	register_method("puppet_sync_position", &NetworkTransform::puppet_sync_position, GODOT_METHOD_RPC_MODE_PUPPET);
	register_method("puppet_sync_position_child", &NetworkTransform::puppet_sync_position_child, GODOT_METHOD_RPC_MODE_PUPPET);
	register_method("puppet_sync_rotation", &NetworkTransform::puppet_sync_rotation, GODOT_METHOD_RPC_MODE_PUPPET);
	register_method("puppet_sync_rotation_child", &NetworkTransform::puppet_sync_rotation_child, GODOT_METHOD_RPC_MODE_PUPPET);
	register_method("puppet_sync_scale", &NetworkTransform::puppet_sync_scale, GODOT_METHOD_RPC_MODE_PUPPET);
	register_method("puppet_sync_scale_child", &NetworkTransform::puppet_sync_scale_child, GODOT_METHOD_RPC_MODE_PUPPET);
	register_method("sync", &NetworkTransform::sync);
	register_method("_on_network_start", &NetworkTransform::_on_network_start);
	register_method("_process", &NetworkTransform::_process);
}

void NetworkTransform::sync()
{
	Node* parent = get_parent();
	if (sync_position)
		rpc_unreliable("puppet_sync_position", parent->get("translation"));
	if (sync_rotation)
		rpc_unreliable("puppet_sync_rotation", parent->get("rotation_degrees"));
	if (sync_scale)
		rpc_unreliable("puppet_sync_scale", parent->get("scale"));

	for (int i = 0; i < children_to_sync.size(); ++i) {
		uint8_t index = i;
		Node* child = get_node(children_to_sync[i]);
		if (sync_position)
			rpc_unreliable("puppet_sync_position_child", index, child->get("translation"));
		if (sync_rotation)
			rpc_unreliable("puppet_sync_rotation_child", index, child->get("rotation_degrees"));
		if (sync_scale)
			rpc_unreliable("puppet_sync_scale", index, child->get("scale"));
	}
}

void NetworkTransform::_on_network_start()
{
	get_parent()->rset_config("global_transform", GODOT_METHOD_RPC_MODE_PUPPET);
	for (int i = 0; i < children_to_sync.size(); ++i) {
		Node* child = get_node(children_to_sync[i]);
		child->rset_config("global_transform", GODOT_METHOD_RPC_MODE_PUPPET);
	}

	if (!is_master()) {
		Node* parent = get_parent();
		parent->set_process(false);
		parent->set_physics_process(false);
		for (int i = 0; i < children_to_sync.size(); ++i) {
			Node* child = get_node(children_to_sync[i]);
			child->set_process(false);
			child->set_physics_process(false);
		}

		rpc_id(get_network_master(), "master_initial_sync");
	} else {
		set_process(true);
	}

}

void NetworkTransform::_init()
{
}

void NetworkTransform::_process(float delta)
{
	current_time += delta;
	if (current_time >= sync_interval) {
		current_time -= sync_interval;
		sync();
	}
}

NetworkTransform::NetworkTransform()
	: children_to_sync{}, sync_interval{0.033f}
	, sync_position{true}, sync_rotation{true}, sync_scale{true}
	, current_time{0}
{
}

void NetworkTransform::master_initial_sync()
{
	int64_t id = get_tree()->get_rpc_sender_id();
	Node* parent = get_parent();
	if (sync_position)
		rpc_id(id, "puppet_sync_position", parent->get("translation"));
	if (sync_rotation)
		rpc_id(id, "puppet_sync_rotation", parent->get("rotation_degrees"));
	if (sync_scale)
		rpc_id(id, "puppet_sync_scale", parent->get("scale"));

	for (int i = 0; i < children_to_sync.size(); ++i) {
		uint8_t index = i;
		Node* child = get_node(children_to_sync[i]);
		if (sync_position)
			rpc_id(id, "puppet_sync_position_child", index, child->get("translation"));
		if (sync_rotation)
			rpc_id(id, "puppet_sync_rotation_child", index, child->get("rotation_degrees"));
		if (sync_scale)
			rpc_id(id, "puppet_sync_scale", index, child->get("scale"));
	}
}

void NetworkTransform::puppet_sync_position(Vector3 pos)
{
	get_parent()->set("translation", pos);
}

void NetworkTransform::puppet_sync_position_child(uint8_t index, Vector3 pos)
{
	Node* child = get_node(children_to_sync[index]);
	child->set("translation", pos);
}

void NetworkTransform::puppet_sync_rotation(Vector3 euler)
{
	get_parent()->set("rotation_degrees", euler);
}

void NetworkTransform::puppet_sync_rotation_child(uint8_t index, Vector3 euler)
{
	Node* child = get_node(children_to_sync[index]);
	child->set("rotation_degrees", euler);
}

void NetworkTransform::puppet_sync_scale(Vector3 scale)
{
	get_parent()->set("scale", scale);
}

void NetworkTransform::puppet_sync_scale_child(uint8_t index, Vector3 scale)
{
	Node* child = get_node(children_to_sync[index]);
	child->set("scale", scale);
}
