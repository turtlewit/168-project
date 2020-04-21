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

	REGISTER_METHOD(Player, set_state);
	REGISTER_METHOD(Player, stop);

	REGISTER_METHOD(Player, _on_HitboxGround_body_entered);
	REGISTER_METHOD(Player, _on_HitboxGround_body_exited);
	REGISTER_METHOD(Player, _on_HitboxCeiling_body_entered);

	register_property("speed", &Player::speed, 4.0f);
	register_property("gravity", &Player::gravity, 3.0f);
	register_property("jump_force", &Player::jump_force, 1.5f);
	register_property("mouse_sensitivity", &Player::mouse_sensitivity, 0.25f);
}

CLASS_INITS(Player)


void Player::_ready()
{
	inp = Input::get_singleton();
	inp->set_mouse_mode(Input::MOUSE_MODE_CAPTURED);

	camera = GET_NODE(Camera, "CameraPivot/Camera");
	camera_pivot = GET_NODE(Position3D, "CameraPivot");
	model = GET_NODE(MeshInstance, "Model");
	attack_box = GET_NODE(CollisionShape, "Model/AttackBox/CollisionShape");
	anim_player = GET_NODE(AnimationPlayer, "AnimationPlayer"); // @TODO: Change to an AnimationTree when we get that system in place
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
	if (state != State::Attack && state != State::Pounce) {
		move_direction = Vector3{ 0, 0, 0 };
		const Transform camera_xform = camera->get_global_transform();

		move_direction += -camera_xform.basis.z * inp->get_action_strength("move_forward");
		move_direction += camera_xform.basis.z * inp->get_action_strength("move_back");
		move_direction += -camera_xform.basis.x * inp->get_action_strength("move_left");
		move_direction += camera_xform.basis.x * inp->get_action_strength("move_right");
	}
	
	if (inp->is_action_just_pressed("move_jump")) {
		jump_buffer = 0;
		if (jump_buffer < JumpBufferLimit && (state == State::Ground || state == State::Air)) {
			jump_buffer++;
			if (jumps < 2)
				jump();
		}
	}

	if (inp->is_action_just_pressed("attack_claw") && state == State::Ground) {
		stop();
		state = State::Attack;
		anim_player->play("Attack");
	}

	if (inp->is_action_just_pressed("attack_pounce") && state == State::Ground) {
		stop();
		state = State::Pounce;

		float rot = model->get_rotation().y;
		move_direction = -Vector3{ std::sin(rot), 0, std::cos(rot) } * PounceStrength; //Horizontal
		y_velocity = jump_force / PounceHeightDivide; //Vertical

		attack_box->set_disabled(false);
	}

	if (state == State::Pounce) {
		const Transform camera_xform = camera->get_global_transform();
		move_direction += -camera_xform.basis.x * inp->get_action_strength("move_left") / PounceTurnPenalty;
		move_direction += camera_xform.basis.x * inp->get_action_strength("move_right") / PounceTurnPenalty;
	}
	
	if (inp->is_action_just_pressed("sys_quit"))
		get_tree()->quit();

	//Godot::print(Variant{ colliding_with });
}


void Player::_physics_process(float delta)
{
	if (Mathf::abs(camera_joy_value) > 0.1) {
		const Vector3 camera_rotation = camera_pivot->get_rotation_degrees();
		camera_pivot->set_rotation_degrees(Vector3{camera_rotation.x, camera_rotation.y - camera_joy_value, camera_rotation.z});
	}

	if (colliding_with == 0)
		y_velocity -= gravity * delta;

	move_direction.y = y_velocity;

	if (is_moving()) {
		Vector3 rot = model->get_rotation();
		target_rotation = (std::atan2(move_direction.x, move_direction.z) + Mathf::deg2rad(180));
		rot.y = Mathf::lerp_delta(rot.y, rot.y + get_closest_angle(fmod(Mathf::abs(rot.y), 2 * Mathf::Pi), target_rotation, rot.y < 0), 0.0005f, delta);
		model->set_rotation(rot);
	}

	move_and_slide(move_direction * speed, Vector3{0, 1, 0});
}


inline bool Player::is_moving()
{
	return move_direction.x != 0 || move_direction.z != 0;
}


inline float Player::get_closest_angle(float current, float target, bool flip) {
	current = (flip == true ? (2 * Mathf::Pi) - current : current);
	float result = target - current;
	if (result > Mathf::Pi)
		result -= (2 * Mathf::Pi);
	else if (result < -Mathf::Pi)
		result += (2 * Mathf::Pi);
	if (result == (2 * Mathf::Pi))
		result = 0;
	return result;
}


void Player::jump()
{
	y_velocity = jump_force;
	jumps++;
	state = State::Air;
}


void Player::land()
{
	y_velocity = 0;
	jumps = 0;
	colliding_with++;
	if (state == State::Pounce) {
		attack_box->set_disabled(true);
	}

	state = State::Ground;
}


void Player::stop()
{
	move_direction = Vector3{ 0, 0, 0 };
}


void Player::set_state(int value)
{
	state = static_cast<State>(value);
}


void Player::_on_HitboxGround_body_entered(Node* body)
{
	if (move_direction.y < 0) {
		land();
	}
}


void Player::_on_HitboxGround_body_exited(Node* body)
{
	jumps = 1;
	colliding_with--;
	if (state != State::Attack && state != State::Pounce)
		state = State::Air;
}


void Player::_on_HitboxCeiling_body_entered(Node* body)
{
	if (y_velocity > 0) {
		y_velocity = 0;
		state = State::Air;
	}
}


void Player::_on_Hurtbox_area_entered(Area* area)
{
	Player* other = cast_to<Player>(area->get_parent());

	switch (other->state) {
		case State::Attack:
			health -= other->attack_power + 1;
			break;
		case State::Pounce:
			health -= other->attack_power;
			break;
	}
}
