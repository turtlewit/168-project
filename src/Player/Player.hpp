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

		Vector3 direction;
		float y_velocity = 0;
		bool on_floor = false;
		bool can_jump = false;

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
		void _on_HitboxGround_body_entered(Node* body);
		void _on_HitboxGround_body_exited(Node* body);
	};

}
