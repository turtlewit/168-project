// Crystal.hpp
#pragma once

#include <Godot.hpp>
#include <Area.hpp>
#include <AnimationPlayer.hpp>

namespace godot {

	class Crystal : public Area {
		GODOT_CLASS(Crystal, Area)

	public:
		enum class Powerup {
			Speed,
			Jump,
			PounceRange,
			SwipeDamage,
			PounceDamage
		};

	private:
		int powerup_type;
		Powerup powerup_type_internal;

		bool collected = false;

		AnimationPlayer* anim_player_dissolve;

	public:
		static void _register_methods();

		Crystal();
		~Crystal();
		void _init();

		void _ready();

	private:
		void _on_Crystal_body_entered(Node* body);
		void _on_AnimationPlayerDissolve_animation_finished(String anim_name);
	};

}
