// Player.hpp
#pragma once

#include <Godot.hpp>
#include <KinematicBody.hpp>
#include <Camera.hpp>
#include <Position3D.hpp>
#include <MeshInstance.hpp>
#include <InputEvent.hpp>
#include <AnimationPlayer.hpp>
#include <Area.hpp>
#include <CollisionShape.hpp>
#include <KinematicCollision.hpp>


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
		static constexpr float PounceHeightDivide = 3.0f;
		static constexpr float PounceTurnPenalty = 20.0f;
		static constexpr float SnapLength = 0.1f;
		static constexpr unsigned int JumpBufferLimit = 7;

		// Growable stats
		unsigned int max_health = 3;
		int health = max_health;
		////////////////////////////
		float speed = 4.0f;
		float jump_force = 4.0f;
		////////////////////////////
		float pounce_strength = 2.0f;
		int swipe_damage = 2;
		int pounce_damage = 1;

		float gravity = 9.8f;
		unsigned int jump_buffer = JumpBufferLimit;
		float mouse_sensitivity = 0.25f;

		Vector3 move_direction;
		float snap_length = 0;
		float velocity = 0;
		float y_velocity = 0;
		int jumps = 0;
		Vector3 prev_pos;
		Ref<KinematicCollision> current_collision;

		float target_rotation = 0;
		float camera_joy_value = 0;

		State state = State::Air;

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

		void increase_speed(float amount);
		void increase_jump(float amount);
		void increase_pounce(float amount);
		void increase_swipe_damage(int amount);
		void increase_pounce_damage(int amount);

	private:
		inline bool is_moving();
		inline float get_closest_angle(float current, float target, bool flip);
		void jump();
		void land();
		void stop();

		void set_state(int value);

		void enter_ground();
		void exit_ground();
		void enter_ceiling();
		void _on_Hurtbox_area_entered(Area* area);
	};

}
