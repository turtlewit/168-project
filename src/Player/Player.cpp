// Player.cpp
#include "Player.hpp"

#include "Utils/Defs.hpp"
#include "Utils/Mathf.hpp"

#include <Input.hpp>
#include <InputEventMouseMotion.hpp>
#include <InputEventJoypadMotion.hpp>
#include <SceneTree.hpp>

using namespace godot;

namespace {
	Input* inp;
}

void Player::_register_methods()
{
	REGISTER_METHOD(Player, _ready);
	REGISTER_METHOD(Player, _process);
	REGISTER_METHOD(Player, _physics_process);
	REGISTER_METHOD(Player, _input);
	REGISTER_METHOD(Player, _on_HitboxGround_body_entered);
	REGISTER_METHOD(Player, _on_HitboxGround_body_exited);

	register_property<Player, float>("speed", &Player::speed, 4.0f);
	register_property<Player, float>("gravity", &Player::gravity, 3.0f);
	register_property<Player, float>("jump_force", &Player::jump_force, 1.5f);
	register_property<Player, float>("mouse_sensitivity", &Player::mouse_sensitivity, 0.25f);
}

CLASS_INITS(Player)


void Player::_ready()
{
	inp = Input::get_singleton();
	inp->set_mouse_mode(Input::MOUSE_MODE_CAPTURED);

	camera = GET_NODE(Camera, "CameraPivot/Camera");
	camera_pivot = GET_NODE(Position3D, "CameraPivot");
	model = GET_NODE(MeshInstance, "MeshInstance");
}


void Player::_input(InputEvent* event)
{
	auto* ev_mouse = cast_to<InputEventMouseMotion>(event);
	if (ev_mouse) {
		const Vector3 camera_rotation = camera_pivot->get_rotation_degrees();
		camera_pivot->set_rotation_degrees(Vector3{camera_rotation.x, camera_rotation.y - mouse_sensitivity * ev_mouse->get_relative().x, camera_rotation.z});
	}

	auto* ev_joystick = cast_to<InputEventJoypadMotion>(event);
	if (ev_joystick && ev_joystick->get_axis() == 2)
		camera_joy_value = ev_joystick->get_axis_value();
}


void Player::_process(float delta)
{
	move_direction = Vector3{0, 0, 0};
	const Transform camera_xform = camera->get_global_transform();

	move_direction += -camera_xform.basis.z * inp->get_action_strength("move_forward");
	move_direction += camera_xform.basis.z * inp->get_action_strength("move_back");
	move_direction += -camera_xform.basis.x * inp->get_action_strength("move_left");
	move_direction += camera_xform.basis.x * inp->get_action_strength("move_right");

	if (inp->is_action_just_pressed("move_jump")) {
		if (on_ground || jumps < 2)
		y_velocity = jump_force;
		jumps++;
		on_ground = false;
	}
	
	if (inp->is_action_just_pressed("sys_quit"))
		get_tree()->quit();
}


void Player::_physics_process(float delta)
{
	if (Mathf::abs(camera_joy_value) > 0.1) {
		const Vector3 camera_rotation = camera_pivot->get_rotation_degrees();
		camera_pivot->set_rotation_degrees(Vector3{camera_rotation.x, camera_rotation.y - camera_joy_value, camera_rotation.z});
	}

	if (!on_ground)
		y_velocity -= gravity * delta;

	move_direction.y = y_velocity;

	if (is_moving()) {
		target_rotation = std::atan2(move_direction.x, move_direction.z) + Mathf::deg2rad(90);

		Vector3 rot = model->get_rotation();
		rot.y = Mathf::lerp_delta(rot.y, target_rotation, 0.0005f, delta);
		model->set_rotation(rot);
	}

	move_and_slide(move_direction * speed, Vector3{0, 1, 0});
}


inline bool Player::is_moving()
{
	return move_direction.x != 0 || move_direction.z != 0;
}


void Player::_on_HitboxGround_body_entered(Node* body)
{
	if (body->is_in_group("Ground")) {
		y_velocity = 0;
		jumps = 0;
		on_ground = true;
	}
}


void Player::_on_HitboxGround_body_exited(Node* body)
{
	if (body->is_in_group("Ground")) {
		jumps = 1;
		on_ground = false;
	}
}
