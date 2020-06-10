// Player.hpp
#pragma once

#include <Godot.hpp>
#include <SceneTree.hpp>
#include <KinematicBody.hpp>
#include <Camera.hpp>
#include <World.hpp>
#include <PhysicsDirectSpaceState.hpp>
#include <Position3D.hpp>
#include <MeshInstance.hpp>
#include <InputEvent.hpp>
#include <AnimationPlayer.hpp>
#include <AnimationTree.hpp>
#include <Area.hpp>
#include <CollisionShape.hpp>
#include <KinematicCollision.hpp>
#include <ShaderMaterial.hpp>
#include <Timer.hpp>
#include <ResourceLoader.hpp>
#include <PackedScene.hpp>

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
		static constexpr unsigned int DeathPlaneDamage = 1;

		bool in_menu = false;

		// Growable Stats
		unsigned int max_health = 5;
		int health = max_health;
		////////////////////////////
		float speed = 4.0f;
		float jump_force = 4.0f;
		////////////////////////////
		float pounce_strength = 2.0f;
		int swipe_damage = 2;
		int pounce_damage = 1;
		int jumps = 0;

		//Control Parameters
		float mouse_sensitivity_x = 0.25f;
		float mouse_sensitivity_y = 0.15f;
		float camera_joy_value_x;
		float camera_joy_value_y;

		//Camera Collision
		float camera_distance;
		float camera_newdistance;
		float distance_multiplier;
		float camera_curdistance;
		Array camera_exclusions;
		Vector3 pivot_location;
		Vector3 camera_location;
		Vector3 camera_direction;
		Vector3 camera_newposition;
		Dictionary camera_result;

		//Movement/Control
		unsigned int jump_buffer = JumpBufferLimit;
		bool can_swipe = true;
		bool can_pounce = true;
		float gravity = 9.8f;
		float snap_length = 0;
		float target_rotation = 0;
		float move_check_rotation;
		bool move_check;
		Vector3 move_direction;
		Vector3 move_output;
		Vector3 gravity_velocity;
		Vector3 ground_normal;
		Ref<KinematicCollision> current_collision;

		bool dead = false;

		Timer* timer_swipe;
		Timer* timer_pounce;
		Timer* timer_respawn;

		State state = State::Air;

		Camera* camera;
		Position3D* camera_pivot;
		PhysicsDirectSpaceState* camera_raycast;
		Spatial* model;
		CollisionShape* attack_box;
		CollisionShape* hit_box;
		AnimationPlayer* anim_player;
		AnimationTree* anim_tree;

		//Sync Shader
		Ref<ShaderMaterial> water_shader;
		float shader_time = 0;

		Ref<PackedScene> pause_menu = ResourceLoader::get_singleton()->load("res://Prefabs/UI/PauseMenu.tscn");

	public:
		static void _register_methods();

		Player();
		~Player();
		void _init();

		void _ready();

		void _exit_tree();

		void _input(InputEvent* event);

		void _process(float delta);
		void _physics_process(float delta);

		void increase_speed(float amount);
		void increase_jump(float amount);
		void increase_pounce(float amount);
		void increase_swipe_damage(int amount);
		void increase_pounce_damage(int amount);
		void set_gravity_velocity(float amount);
		void damage(int amount);

	private:
		bool is_master() { return get_tree()->get_network_unique_id() == get_network_master(); }
		inline bool is_moving();
		inline float get_closest_angle(float current, float target, bool flip);
		void jump();
		void land();
		void stop();

		void set_state(int value);

		
		void respawn();
		void enter_ground();
		void exit_ground();
		void check_ground();
		void enter_ceiling();
		void _on_Hurtbox_area_entered(Area* area);
		void check_camera();
		void adjust_camera(Vector3 from, Vector3 to, float lenfrom, float lento);
		void check_water();
		void check_deathplane();
		void rpc_set_anim_param(String name, Variant to)
		{
			rpc("puppet_set_anim_param", Array::make(name, to));
		}

		void puppet_set_anim_param(String name, Variant to);

		void _on_player_hit(int64_t net_id, int64_t damage);

		void _on_TimerSwipe_timeout();
		void _on_TimerPounce_timeout();

		void _on_menu_closed();
	};

}
