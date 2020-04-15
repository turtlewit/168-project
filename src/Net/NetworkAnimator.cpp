#include <AnimationPlayer.hpp>
#include <SceneTree.hpp>

#include "Net/NetworkAnimator.hpp"

using namespace godot;

void NetworkAnimator::_register_methods()
{
	NetworkNode<Node, NetworkAnimator>::_register_methods();

	register_method("master_initial_sync", &NetworkAnimator::master_initial_sync, GODOT_METHOD_RPC_MODE_MASTER);
	register_method("puppet_start_animation", &NetworkAnimator::puppet_start_animation, GODOT_METHOD_RPC_MODE_PUPPET);
	register_method("puppet_stop_animation", &NetworkAnimator::puppet_stop_animation, GODOT_METHOD_RPC_MODE_PUPPET);
	register_method("_on_animator_animation_finished", &NetworkAnimator::_on_animator_animation_finished);
	register_method("_on_animator_animation_changed", &NetworkAnimator::_on_animator_animation_changed);
	register_method("_on_animator_animation_started", &NetworkAnimator::_on_animator_animation_started);

	register_property("animator_path", &NetworkAnimator::animator_path, NodePath());
}

void NetworkAnimator::_ready()
{
	NetworkNode<Node, NetworkAnimator>::_ready();

	animator = cast_to<AnimationPlayer>(get_node(animator_path));

	animation_names = animator->get_animation_list();

	for (int i = 0; i < animation_names.size(); ++i) {
		animation_dictionary[animation_names[i]] = i;
	}
}

void NetworkAnimator::_on_network_start()
{
	if (is_master()) {
		animator->connect("animation_changed", this, "_on_animator_animation_changed");
		animator->connect("animation_finished", this, "_on_animator_animation_finished");
		animator->connect("animation_started", this, "_on_animator_animation_started");
	} else {
		rpc_id(get_network_master(), "master_initial_sync");
	}
}

void NetworkAnimator::_on_animator_animation_changed(String old_name, String new_name)
{
	//rpc("puppet_start_animation", animation_dictionary[new_name]);
}

void NetworkAnimator::_on_animator_animation_finished(String name)
{
	rpc("puppet_stop_animation");
}

void NetworkAnimator::_on_animator_animation_started(String name)
{
	rpc("puppet_start_animation", animation_dictionary[name], animator->get_current_animation_position());
}

void NetworkAnimator::_init()
{
}

NetworkAnimator::NetworkAnimator()
	: animator_path{}, animator{ nullptr }, animation_names{}, animation_dictionary{}
{
}

void NetworkAnimator::master_initial_sync()
{
	int64_t id = get_tree()->get_rpc_sender_id();
	if (animator->is_playing()) {
		rpc_id(id, "puppet_start_animation", animation_dictionary[animator->get_current_animation()], animator->get_current_animation_position());
	} else {
		rpc_id(id, "puppet_stop_animation");
	}
}

void NetworkAnimator::puppet_start_animation(int index, float time)
{
	animator->play(animation_names[index]);
	animator->seek(time);
}

void NetworkAnimator::puppet_stop_animation()
{
	animator->stop();
}