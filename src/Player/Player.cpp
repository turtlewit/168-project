// Player.cpp
#include "Player.hpp"

#include "Utils/Defs.hpp"
#include "Utils/Mathf.hpp"
#include "System/SignalManagerPlayer.hpp"
#include "Net/NetworkSignalManager.hpp"
#include "Net/NetworkManager.hpp"
#include "Net/Utils.hpp"
#include "GameManager.hpp"

#include <Input.hpp>
#include <InputEventMouseMotion.hpp>
#include <InputEventJoypadMotion.hpp>
#include <InputEventAction.hpp>
#include <SceneTree.hpp>
#include <CanvasItem.hpp>
#include <Viewport.hpp>
#include <vector>
using namespace godot;

namespace {
	Input* inp;
}

void Player::_register_methods()
{
	register_signal<Player>("player_arena_died", Dictionary());

	REGISTER_METHOD(Player, _ready);
	REGISTER_METHOD(Player, _on_network_connected);
	REGISTER_METHOD(Player, _exit_tree);
	REGISTER_METHOD(Player, _process);
	REGISTER_METHOD(Player, _physics_process);
	REGISTER_METHOD(Player, _input);

	REGISTER_METHOD(Player, set_state);
	REGISTER_METHOD(Player, stop);
	REGISTER_METHOD(Player, respawn);

	REGISTER_METHOD(Player, _on_Hurtbox_area_entered);
	REGISTER_METHOD(Player, _on_TimerSwipe_timeout);
	REGISTER_METHOD(Player, _on_TimerPounce_timeout);
	REGISTER_METHOD(Player, _on_menu_closed);
	REGISTER_METHOD(Player, _on_game_manager_state_changed);

	register_method("puppet_set_anim_param", &Player::puppet_set_anim_param, GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	register_method("server_arena_died", &Player::server_arena_died, GODOT_METHOD_RPC_MODE_REMOTE);
	register_method("set_collider_disabled", &Player::set_collider_disabled, GODOT_METHOD_RPC_MODE_REMOTESYNC);
	register_method("set_namesprite_visible", &Player::set_namesprite_visible, GODOT_METHOD_RPC_MODE_REMOTE);
	register_method("set_my_username", &Player::set_my_username, GODOT_METHOD_RPC_MODE_REMOTE);

	REGISTER_METHOD(Player, _on_player_hit);
	REGISTER_METHOD(Player, set_gravity_velocity);
	register_property("speed", &Player::speed, 4.0f);
	register_property("gravity", &Player::gravity, 9.8f);
	register_property("jump_force", &Player::jump_force, 4.0f);
	register_property("mouse_sensitivity_x", &Player::mouse_sensitivity_x, 0.25f);
	register_property("mouse_sensitivity_y", &Player::mouse_sensitivity_y, 0.25f);
	register_property("username", &Player::set_username, &Player::get_username, String(), GODOT_METHOD_RPC_MODE_PUPPET);
	register_property("dead", &Player::dead, false, GODOT_METHOD_RPC_MODE_REMOTE, static_cast<godot_property_usage_flags>(0));
}

CLASS_INITS(Player)


void Player::_ready()
{
	inp = Input::get_singleton();
	inp->set_mouse_mode(Input::MOUSE_MODE_CAPTURED);

	camera = GET_NODE(Camera, "CameraPivot/Camera");
	camera_pivot = GET_NODE(Position3D, "CameraPivot");
	model = GET_NODE(Spatial, "PlayerModel");
	attack_box = GET_NODE(CollisionShape, "PlayerModel/AttackBox/CollisionShape");
	hit_box = GET_NODE(CollisionShape, "CollisionShape");
	anim_player = GET_NODE(AnimationPlayer, "AnimationPlayer");
	anim_tree = GET_NODE(AnimationTree, "AnimationTree");
	sound_jump = GET_NODE(AudioStreamPlayer3D, "SoundJump");
	sound_swipe = GET_NODE(AudioStreamPlayer3D, "SoundSwipe");
	sound_damage = GET_NODE(AudioStreamPlayer3D, "SoundDamage");
	sound_dissolve = GET_NODE(AudioStreamPlayer3D, "SoundDissolve");
	timer_swipe = GET_NODE(Timer, "TimerSwipe");
	timer_pounce = GET_NODE(Timer, "TimerPounce");
	timer_respawn = GET_NODE(Timer, "TimerRespawn");

	MeshInstance* mesh = GET_NODE(MeshInstance, "PlayerModel/Armature/Skeleton/Animal");
	Ref<Material> mat = mesh->get_material_override()->duplicate();
	mesh->set_material_override(mat);

	water_shader = cast_to<MeshInstance>(get_parent()->get_node("Water/MeshInstance"))->get_surface_material(0);

	camera_distance = camera_pivot->get_global_transform().origin.distance_to(camera->get_global_transform().origin);
	camera_exclusions.append(this);

	NetworkSignalManager::get_singleton()->connect("player_hit", this, "_on_player_hit");
}


void Player::_on_network_connected()
{
	if (IS_MASTER) {
		GameManager::get_singleton()->connect("state_changed", this, "_on_game_manager_state_changed");
		set_username(get_node("/root/LocalPlayerName")->get("username"));
	} else {
		get_node("NameSprite")->set("visible", true);
		rpc_id(get_network_master(), "set_my_username");
	}
}


void Player::_exit_tree()
{
	inp->set_mouse_mode(Input::MOUSE_MODE_VISIBLE);
}


void Player::_input(InputEvent* event)
{
	if (in_menu)
		return;

	if (GameManager::get_singleton()->get_state() == GameManager::State::lobby && IS_SERVER) {
		if (event->is_action_pressed("start_round")) {
			GameManager::get_singleton()->rpc("change_state", static_cast<int>(GameManager::State::collection));
		}
	}

	auto* ev_mouse = cast_to<InputEventMouseMotion>(event);
	if (ev_mouse) {
		const Vector3 camera_rotation = camera_pivot->get_rotation_degrees();
		camera_pivot->set_rotation_degrees(Vector3{camera_rotation.x, 
			camera_rotation.y - mouse_sensitivity_x * ev_mouse->get_relative().x, camera_rotation.z + mouse_sensitivity_y * ev_mouse->get_relative().y });
	}

	//auto* ev_joystick = cast_to<InputEventJoypadMotion>(event);
	//if (ev_joystick) {
	//	if (ev_joystick->get_axis() == 2)
	//		camera_joy_value_x = ev_joystick->get_axis_value();
	//	if (ev_joystick->get_axis() == 1)
	//		camera_joy_value_y = ev_joystick->get_axis_value();

	//	if (Mathf::abs(camera_joy_value_x) > 0.1 || Mathf::abs(camera_joy_value_y) > 0.1) {
	//		const Vector3 camera_rotation = camera_pivot->get_rotation_degrees();
	//		camera_pivot->set_rotation_degrees(Vector3{ camera_rotation.x, camera_rotation.y - camera_joy_value_x, camera_rotation.z + camera_joy_value_y });
	//	}
	//}

}


void Player::_process(float delta)
{
	water_shader->set_shader_param("deltaTime", shader_time);
	shader_time += delta;

	if (state != State::Pounce && !dead) {
		move_direction = Vector3{ 0, 0, 0 };
		const Transform camera_xform = camera->get_global_transform();
		move_check_rotation = fmod(Mathf::abs(camera_pivot->get_rotation_degrees().z), 360);
		move_check = camera_pivot->get_rotation_degrees().z > 0 ? 
			(move_check_rotation >= 65 && move_check_rotation < 155) : (move_check_rotation >= 205 && move_check_rotation < 295);
		move_direction += -camera_xform.basis.z * (move_check == true ? inp->get_action_strength("move_back") : inp->get_action_strength("move_forward"));
		move_direction += camera_xform.basis.z * (move_check == true ? inp->get_action_strength("move_forward") : inp->get_action_strength("move_back"));
		move_direction += -camera_xform.basis.x * inp->get_action_strength("move_left");
		move_direction += camera_xform.basis.x * inp->get_action_strength("move_right");

		move_direction.y = 0;
	}

	if (inp->is_action_just_pressed("move_jump") && !dead && (state == State::Ground || state == State::Air)) {
		jump_buffer = 0;
		snap_length = 0;
		if (jumps < 2)
			jump();
	}

	if (jump_buffer < JumpBufferLimit) {
		jump_buffer++;
		if (state == State::Ground && jumps == 0) {
			snap_length = 0;
			jump();
		}
	}
	
	if (inp->is_action_just_pressed("attack_claw") && !dead && can_swipe && state == State::Ground) {
		stop();
		sound_swipe->play();
		state = State::Attack;
		anim_player->play("Attack");
		//anim_tree->set("parameters/swipe/active", true);
		rpc_set_anim_param("parameters/swipe/active", true);

		can_swipe = false;
		timer_swipe->start();
	}

	if (inp->is_action_just_pressed("attack_pounce") && !dead && can_pounce && state == State::Ground) {
		stop();
		sound_swipe->play();
		state = State::Pounce;

		float rot = model->get_rotation().y;
		move_direction = -Vector3{ std::sin(rot), 0, std::cos(rot) } * pounce_strength; //Horizontal
		snap_length = 0;
		gravity_velocity = Vector3{ 0, (pounce_strength + (jump_force/2) ) / PounceHeightDivide, 0 }; //Vertical

		attack_box->set_disabled(false);

		can_pounce = false;
		SignalManagerPlayer::get_singleton()->emit_signal("player_pounced");
		timer_pounce->start();
	}

	if (state == State::Pounce) {
		const Transform camera_xform = camera->get_global_transform();
		move_direction += -camera_xform.basis.x * inp->get_action_strength("move_left") / PounceTurnPenalty;
		move_direction += camera_xform.basis.x * inp->get_action_strength("move_right") / PounceTurnPenalty;
	}

	float transition_speed = 4;
	if (is_moving() && walk_amount < 1.0f) {
		walk_amount += delta * transition_speed;
		if (walk_amount > 1.0f) {
			walk_amount = 1.0f;
		}
	} else if (walk_amount > 0.0f) {
		walk_amount -= delta * transition_speed;
		if (walk_amount < 0.0f) {
			walk_amount = 0.0f;
		}
	}

	//walk_amount = Mathf::lerp_delta(walk_amount, is_moving() ? 1.0f : 0.0f, 1, delta);
	rpc_set_anim_param("parameters/walk_blend/blend_amount", state != State::Air && !anim_tree->get("parameters/jump_end/active") ? walk_amount : 0.0f);

	//anim_tree->set("parameters/walk_blend/blend_amount", state != State::Air && !anim_tree->get("parameters/jump_end/active") ? move_direction.length() : 0.0f);
	//rpc_set_anim_param("parameters/walk_blend/blend_amount", state != State::Air && !anim_tree->get("parameters/jump_end/active") ? move_direction.length() : 0.0f);
	
	if (inp->is_action_just_pressed("sys_quit") && !in_menu) {
		inp->set_mouse_mode(Input::MOUSE_MODE_VISIBLE);
		in_menu = true;
		auto menu = pause_menu->instance();
		menu->connect("menu_closed", this, "_on_menu_closed");
		get_tree()->get_root()->add_child(menu);
	}

	

	check_deathplane();
}

void Player::_physics_process(float delta)
{
	if (state != State::Ground && state != State::Attack)
		gravity_velocity -= Vector3{ 0, gravity * delta, 0 }; //Apply constant gravity on player

	move_direction = move_direction.normalized() * speed;

	if (is_moving()) { //If you are moving, rotate direction of player model
		Vector3 rot = model->get_rotation();
		target_rotation = (std::atan2(move_direction.x, move_direction.z) + Mathf::deg2rad(180));
		rot.y = Mathf::lerp_delta(rot.y, rot.y + get_closest_angle(fmod(Mathf::abs(rot.y), 2 * Mathf::Pi), target_rotation, rot.y < 0), 0.0005f, delta);
		model->set_rotation(rot);
		hit_box->set_rotation(rot);
	}

	if (!dead) {
		check_ground(); //Checks if you are on non-flat ground and adjusts gravity towards it
		move_output = Vector3(move_direction.x + gravity_velocity.x, gravity_velocity.y, move_direction.z + gravity_velocity.z);
		move_output = move_and_slide_with_snap(move_output, -ground_normal * snap_length, Vector3{ 0, 1, 0 }, true); //Final result
	}

	check_camera();

	if (is_on_ceiling() && state != State::Ground) {
		enter_ceiling();
	}
	if (is_on_floor())
		enter_ground();
	else exit_ground();
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
	sound_jump->play();
	gravity_velocity = Vector3{ 0, jump_force, 0 };
	jumps++;
	state = State::Air;
	anim_player->play("Jump");
}


void Player::stop()
{
	move_direction = Vector3{ 0, 0, 0 };
}


void Player::set_state(int value)
{
	state = static_cast<State>(value);
}

void Player::increase_speed()
{
	speed += (.25f / log10f(speed));
	crystal_count[0]++;
}

void Player::increase_jump()
{
	jump_force += (.28f / log10f(jump_force));
	crystal_count[1]++;
}

void Player::increase_swipe()
{
	swipe_damage += (1.5f / log10f(swipe_damage));
	crystal_count[2]++;
}

void Player::increase_pounce()
{
	pounce_damage += (4.0f / log10f(pounce_damage));
	pounce_strength += 0.35f;
	crystal_count[3]++;
}

void Player::decrease_speed()
{
	speed -= (.25f / log10f(speed));
}


void Player::decrease_jump()
{
	jump_force -= (.28f / log10f(jump_force));
}

void Player::decrease_swipe()
{
	swipe_damage -= (1.5f / log10f(swipe_damage));
}

void Player::decrease_pounce()
{
	pounce_damage -= (4.0f / log10f(pounce_damage));
	pounce_strength -= 0.35f;
}

void Player::damage(int amount)
{
	if (!dead) {
		sound_damage->play();
		SignalManagerPlayer::get_singleton()->emit_signal("player_damaged", health, amount, get_name());
		health -= amount;
	} else {
		return;
	}

	if (health <= 0) {
		rpc("set_collider_disabled", true);
		dead = true;
		sound_dissolve->play();
		GET_NODE(AnimationPlayer, "AnimationPlayerDissolve")->play("Dissolve");
		if (IS_MASTER)
			rpc("set_namesprite_visible", false);
		std::vector<int> crystals;
		for (int i = 0; i < 4; i++)
		{
			if (crystal_count[i] > 0)
				crystals.push_back(i);
		}
		int ptype = Mathf::rand_range(0, crystals.size());
		if(crystals.size() > 0)
			SignalManagerPlayer::get_singleton()->emit_signal("player_crystal_amount_changed",crystal_count[ptype], -1);
		switch (crystal_count[ptype])
		{
		case 0:
			decrease_speed();
			break;
		case 1:
			decrease_jump();
			break;
		case 2:
			decrease_swipe();
			break;
		case 3:
			decrease_pounce();
			break;
		}
		if (GameManager::get_singleton()->get_state() != GameManager::State::arena)
			timer_respawn->start();
		else {
			if (IS_SERVER)
				server_arena_died();
			else
				rpc_id(NetworkManager::SERVER_ID, "server_arena_died");
		}
	}
}

void Player::set_gravity_velocity(float amount)
{
	gravity_velocity = Vector3(0, amount, 0);
}

void Player::enter_ground()
{
	if (gravity_velocity.y < 0) {
		gravity_velocity = -ground_normal; //Is_on_floor requires you to have a small force pushing down
		jumps = 0;
		if (state == State::Pounce) {
			attack_box->set_disabled(true);
		}
		snap_length = SnapLength;
		if (state != State::Attack && state != State::Ground) {
			state = State::Ground;
			//anim_tree->set("parameters/jump_start/blend_amount", 0.0f);
			//anim_tree->set("parameters/jump_end/active", true);
			rpc_set_anim_param("parameters/jump_start/blend_amount", 0.0f);
			rpc_set_anim_param("parameters/jump_end/active", true);
		}
			
	}
}

void Player::enter_ceiling() 
{
	if (gravity_velocity.y > 0)
		gravity_velocity.y = 0;
}


void Player::exit_ground()
{
	if (jumps == 0)
		jumps = 1;
	if (state != State::Attack && state != State::Pounce)
		state = State::Air;
}


void Player::check_ground() 
{
	for (int64_t i = 0; i < get_slide_count(); ++i) {
		current_collision = get_slide_collision(i);
		Vector3 normal = current_collision->get_normal();
		float angle = acos(normal.dot(Vector3(0, 1, 0))); //in Radians, returns angle of slope
		if (angle > 0 && angle < Mathf::Pi / 4) { // If you are on a ground thats ~0-45degrees
			ground_normal = normal;
			break;
		}
		else {
			ground_normal = Vector3{ 0, 1, 0 };
		}
	}
}

void Player::check_deathplane()
{
	if (get_global_transform().origin.y < -20)
	{
		if (health <= DeathPlaneDamage)
			damage(DeathPlaneDamage);
		else
		{
			damage(DeathPlaneDamage);
			NetworkManager::get_singleton()->spawn_player(this);
		}
	}
}

void Player::respawn()
{
	rpc("set_collider_disabled", false);
	GET_NODE(AnimationPlayer, "AnimationPlayerDissolve")->play("Undissolve");
	if (IS_MASTER)
		rpc("set_namesprite_visible", true);
	NetworkManager::get_singleton()->spawn_player(this);
	health = max_health;
	SignalManagerPlayer::get_singleton()->emit_signal("player_damaged", 0, -static_cast<int>(max_health), get_name());
	dead = false;
}


void Player::_on_Hurtbox_area_entered(Area* area)
{
	Player* other = cast_to<Player>(area->get_node("../.."));
	
	if (other->attack_box->is_disabled() == true)
		return;

	switch (other->state) {
		case State::Attack:
			//damage(other->swipe_damage);
			NetworkSignalManager::get_singleton()->emit_network_signal("player_hit", Array::make(get_network_master(), other->swipe_damage));
			other->attack_box->set_disabled(true);
			if (!dead)
				sound_damage->play();
			break;
		case State::Pounce:
			//damage(other->pounce_damage);
			NetworkSignalManager::get_singleton()->emit_network_signal("player_hit", Array::make(get_network_master(), other->pounce_damage));
			other->attack_box->set_disabled(true);
			if (!dead)
				sound_damage->play();
			break;
		default:
			break;
	}
}

void Player::check_camera() //Handles camera interaction
{
	pivot_location = camera_pivot->get_global_transform().origin;
	camera_location = camera->get_global_transform().origin;
	camera_direction = (camera_location - pivot_location).normalized();
	camera_curdistance = pivot_location.distance_to(camera_location);

	camera_raycast = camera_pivot->get_world()->get_direct_space_state();
	camera_result = camera_raycast->intersect_ray(pivot_location, pivot_location + camera_direction * camera_distance, camera_exclusions);

	if (camera_result.size() != 0) { //If raycast detects something, reduce camera distance accordingly
		camera_newdistance = pivot_location.distance_to(camera_result["position"]) * .8;
		adjust_camera(pivot_location, camera_location, camera_curdistance, camera_newdistance);
	}
	else {
		adjust_camera(pivot_location, camera_location, camera_curdistance, camera_distance);
	}

	check_water();
}

void Player::adjust_camera(Vector3 from, Vector3 to, float lenfrom, float lento) //Adjust camera distance
{
	distance_multiplier = lento / lenfrom;
	camera_newposition = Vector3(from.x + ((to.x - from.x) * distance_multiplier),
		from.y + ((to.y - from.y) * distance_multiplier), from.z + ((to.z - from.z) * distance_multiplier));
	if (lenfrom < lento)
		camera->set_global_transform(Transform(camera->get_global_transform().basis, camera->get_global_transform().origin.linear_interpolate(camera_newposition, 0.08f)));
	else
		camera->set_global_transform(Transform(camera->get_global_transform().basis, camera_newposition));
}

void Player::check_water() {
	if (camera_location.y < 0.6)
	{
		cast_to<CanvasItem>(get_parent()->get_node("GameUI")->get_node("UnderWater"))->show();
		cast_to<CanvasItem>(get_parent()->get_node("GameUI")->get_node("UnderWaterFog"))->show();
	}
	else
	{
		cast_to<CanvasItem>(get_parent()->get_node("GameUI")->get_node("UnderWater"))->hide();
		cast_to<CanvasItem>(get_parent()->get_node("GameUI")->get_node("UnderWaterFog"))->hide();
	}
}

void Player::puppet_set_anim_param(String name, Variant to)
{
	anim_tree->set(name, to);
}

void Player::_on_player_hit(int64_t net_id, int64_t damage)
{
	if (net_id != get_tree()->get_network_unique_id() || !is_master())
		return;

	Player::damage((int) damage);
}

void Player::_on_TimerSwipe_timeout()
{
	//anim_tree->set("parameters/swipe/active", false);
	rpc_set_anim_param("parameters/swipe/active", false);
	can_swipe = true;
}


void Player::_on_TimerPounce_timeout()
{
	can_pounce = true;
}


void Player::_on_menu_closed()
{
	in_menu = false;
	inp->set_mouse_mode(Input::MOUSE_MODE_CAPTURED);
}

void Player::_on_game_manager_state_changed(int state)
{
	GameManager::State s = static_cast<GameManager::State>(state);
	if (s == GameManager::State::lobby || s == GameManager::State::collection || s == GameManager::State::arena) {
		if (s == GameManager::State::lobby || s == GameManager::State::collection)
			NetworkManager::get_singleton()->spawn_player(this);
		else
			spawn_at_arena();

		SignalManagerPlayer::get_singleton()->emit_signal("player_damaged", health, -static_cast<int>(max_health - health), get_name());
		health = max_health;
		if (dead) {
			rpc("set_collider_disabled", false);
			rpc("set_namesprite_visible", true);
			GET_NODE(AnimationPlayer, "AnimationPlayerDissolve")->play("Undissolve");
			dead = false;
		}

		crystal_count[0] = 0;
		crystal_count[1] = 0;
		crystal_count[2] = 0;
		crystal_count[3] = 0;

		speed = initial_speed;
		jump_force = initial_jump_force;
		pounce_strength = initial_pounce_strength;
		swipe_damage = initial_swipe_damage;
		pounce_damage = initial_pounce_damage;

		timer_respawn->stop();
	}
}

void Player::spawn_at_arena()
{
	Node* spawn_points = get_tree()->get_current_scene()->find_node(String("ArenaSpawnPoints"), true, false);
	if (spawn_points) {
		Transform player_transform = get_global_transform();
		player_transform.origin = cast_to<Spatial>(spawn_points->get_children()[Mathf::rand_range(0, 9)])->get_global_transform().origin;
		set_global_transform(player_transform);
	}
}

void Player::server_arena_died()
{
	emit_signal("player_arena_died");
}
