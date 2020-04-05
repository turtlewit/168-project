// Player.hpp
#pragma once

#include <Godot.hpp>
#include <KinematicBody.hpp>
#include <Camera.hpp>
#include <Position3D.hpp>
#include <MeshInstance.hpp>
#include <InputEvent.hpp>

namespace godot {

	class Player : public KinematicBody {
		GODOT_CLASS(Player, KinematicBody)

	private:
		float speed = 4.0f;
		float gravity = 3.0f;
		float jump_force = 1.5f;
		float mouse_sensitivity = 0.25f;

		Vector3 move_direction;
		float y_velocity = 0;
		bool on_ground = false;
		int jumps = 0;

		float target_rotation = 0;
		float camera_joy_value = 0;

		// Children
		Camera* camera;
		Position3D* camera_pivot;
		MeshInstance* model;

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

		void _on_HitboxGround_body_entered(Node* body);
		void _on_HitboxGround_body_exited(Node* body);
	};

}
