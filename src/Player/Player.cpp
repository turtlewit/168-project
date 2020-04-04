// Player.cpp
#include "Player.hpp"

#include "Utils/Defs.hpp"

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
		Vector3 camera_rotation = camera_pivot->get_rotation_degrees();
		camera_pivot->set_rotation_degrees(Vector3{camera_rotation.x, camera_rotation.y - mouse_sensitivity * ev_mouse->get_relative().x, camera_rotation.z});
	}

	auto* ev_joystick = cast_to<InputEventJoypadMotion>(event);
	if (ev_joystick && ev_joystick->get_axis() == 2) {
		Vector3 camera_rotation = camera_pivot->get_rotation_degrees();
		camera_pivot->set_rotation_degrees(Vector3{camera_rotation.x, camera_rotation.y - mouse_sensitivity * ev_joystick->get_axis_value(), camera_rotation.z});
	}
}


void Player::_process(float delta)
{
	direction = Vector3{0, 0, 0};
	const Transform camera_xform = camera->get_global_transform();

	if (inp->is_action_pressed("move_forward"))
		direction += -camera_xform.basis.z;
	if (inp->is_action_pressed("move_back"))
		direction += camera_xform.basis.z;
	if (inp->is_action_pressed("move_left"))
		direction += -camera_xform.basis.x;
	if (inp->is_action_pressed("move_right"))
		direction += camera_xform.basis.x;

	direction.normalize();

	if (inp->is_action_just_pressed("move_jump") && can_jump) {
		y_velocity = jump_force;
		can_jump = false;
		on_floor = false;
	}

	if (!on_floor)
		y_velocity -= gravity * delta;

	direction.y = y_velocity;
	
	if (inp->is_action_just_pressed("sys_quit"))
		get_tree()->quit();
}


void Player::_physics_process(float delta)
{
	move_and_slide(direction * speed, Vector3{0, 1, 0});
}


void Player::_on_HitboxGround_body_entered(Node* body)
{
	if (body->is_in_group("Ground")) {
		y_velocity = 0;
		can_jump = true;
		on_floor = true;
	}
}


void Player::_on_HitboxGround_body_exited(Node* body)
{
	if (body->is_in_group("Ground")) {
		can_jump = false;
		on_floor = false;
	}
}
