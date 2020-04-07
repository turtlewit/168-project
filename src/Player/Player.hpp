// Player.hpp
#pragma once

#include <Godot.hpp>
#include <KinematicBody.hpp>
#include <Camera.hpp>
#include <Position3D.hpp>
#include <MeshInstance.hpp>
#include <InputEvent.hpp>
#include <AnimationPlayer.hpp>
#include <CollisionShape.hpp>

namespace godot {

	class Player : public KinematicBody {
		GODOT_CLASS(Player, KinematicBody)

	public:
		enum class State {
			Ground,
			Air,
			Attack,
			Pounce
		};

	private:
		static constexpr float PounceStrength = 2.0f;
		static constexpr float PounceHeightDivide = 3.0f;
		static constexpr float PounceTurnPenalty = 20.0f;
		static constexpr unsigned int JumpBufferLimit = 10;

		float speed = 4.0f;
		float gravity = 3.0f;
		float jump_force = 1.5f;
		unsigned int jump_buffer = JumpBufferLimit;
		float mouse_sensitivity = 0.25f;
		bool test = true;

		Vector3 move_direction;
		float y_velocity = 0;
		bool on_ground = false;
		int jumps = 0;

		float target_rotation = 0;
		float camera_joy_value = 0;

		State state = State::Ground;

		Camera* camera;
		Position3D* camera_pivot;
		MeshInstance* model;
		CollisionShape* attack_box;
		AnimationPlayer* anim_player; // @TODO: Change to an AnimationTree when we get that system in place

	public:
		static void _register_methods();

		Player();
		~Player();
		void _init();

		void _ready();

		void _input(InputEvent* event);

		void _process(float delta);
		void _physics_process(float delta);

	private:
		inline bool is_moving();
		inline float get_closest_angle(float current, float target, bool flip);
		void jump();
		void land();
		void stop();

		void set_state(int value);

		void _on_HitboxGround_body_entered(Node* body);
		void _on_HitboxGround_body_exited(Node* body);
		void _on_HitboxCeiling_body_entered(Node* body);
	};

}
