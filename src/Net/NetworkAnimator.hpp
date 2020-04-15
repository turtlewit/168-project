#pragma once

#include <Godot.hpp>
#include <Node.hpp>
#include <NodePath.hpp>
#include <PoolArrays.hpp>

#include "Net/NetworkNode.hpp"

namespace godot {
	class AnimationPlayer;
}

class NetworkAnimator : public NetworkNode<godot::Node, NetworkAnimator> {
	GODOT_CLASS(NetworkAnimator, godot::Node)

public:
	static void _register_methods();

	void _ready();
	void _on_network_start() override;

	void _on_animator_animation_changed(godot::String old_name, godot::String new_name);
	void _on_animator_animation_finished(godot::String name);
	void _on_animator_animation_started(godot::String name);

	void _init();

	NetworkAnimator();

private:
	void master_initial_sync();
	void puppet_start_animation(int index, float time);
	void puppet_stop_animation();

	godot::NodePath animator_path;
	godot::AnimationPlayer* animator;

	godot::PoolStringArray animation_names;
	godot::Dictionary animation_dictionary;
};
