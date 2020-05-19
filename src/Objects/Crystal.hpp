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
			Damage
		};

	private:
		int powerup_type;
		Powerup powerup_type_internal;

		bool collected = false;
		float respawn_time = 10.0f;
		AnimationPlayer* anim_player_dissolve;
		AnimationPlayer* anim_player_respawn;

	public:
		static void _register_methods();

		Crystal();
		~Crystal();
		void _init();

		void _ready();

	private:
		void respawn();
		void set_color();
		void _on_Crystal_body_entered(Node* body);
	};

}
