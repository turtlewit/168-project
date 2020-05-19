// Crystal.cpp
#include "Crystal.hpp"

#include "Utils/Defs.hpp"
#include "Player/Player.hpp"
#include "System/SignalManagerPlayer.hpp"
#include <Timer.hpp>
#include <random>
using namespace godot;

void Crystal::_register_methods()
{
	REGISTER_METHOD(Crystal, _ready);
	REGISTER_METHOD(Crystal, _process);

	REGISTER_METHOD(Crystal, respawn);

	REGISTER_METHOD(Crystal, _on_Crystal_body_entered);
	REGISTER_METHOD(Crystal, _on_AnimationPlayerDissolve_animation_finished);

	REGISTER_PROPERTY_HINT(Crystal, int, powerup_type, 0, GODOT_PROPERTY_HINT_ENUM, "Speed,Jump,Pounce Range,Damage");

	register_property("respawn_time", &Crystal::respawn_time, (real_t)10);
	register_signal<Crystal>("picked_up", Dictionary{});
}

CLASS_INITS(Crystal)


void Crystal::_ready()
{
	anim_player_dissolve = GET_NODE(AnimationPlayer, "AnimationPlayerDissolve");
	anim_player_respawn = GET_NODE(AnimationPlayer, "AnimationPlayerRespawn");
	powerup_type_internal = static_cast<Powerup>(powerup_type);
}
void Crystal::_process(float delta)
{
	Timer* t = cast_to<Timer>(get_node("Timer"));
	if (t->is_stopped() && collected)
		respawn();
}

void Crystal::_on_Crystal_body_entered(Node* body)
{
	if (!collected && body->is_in_group("Player")) {
		anim_player_dissolve->play("Dissolve");
		collected = true;
		
		Player* player = cast_to<Player>(body);
		switch (powerup_type_internal) {
			case Powerup::Speed:
				player->increase_speed(1);
				break;
			case Powerup::Jump:
				player->increase_jump(0.5f);
				break;
			case Powerup::PounceRange:
				player->increase_pounce(0.5f);
				break;
			case Powerup::Damage: {
				player->increase_swipe_damage(1);
				player->increase_pounce_damage(1);
			} break;
		}
		Timer* t = cast_to<Timer>(get_node("Timer"));
		t->start(respawn_time);
		SignalManagerPlayer::get_singleton()->emit_signal("player_crystal_amount_changed", static_cast<int>(powerup_type_internal), 1);
	}
}

void Crystal::respawn()
{
	//{ "BLUE": #00c0ff 8ce3ff, "YELLOW" : #fffc00 8adf00,  "ORANGE" : #ff6c00 ffaf74, "PURPLE" : #c000ff d5a7de}
	Godot::print("Respawn");
	collected = false;
	switch (rand() % 4)
	{
		case 0:
			powerup_type_internal = Powerup::Speed;
			break;
		case 1:
			powerup_type_internal = Powerup::Jump;
			break;
		case 2:
			powerup_type_internal = Powerup::Damage;
			break;
		case 3:
			powerup_type_internal = Powerup::PounceRange;
			break;
	}
	anim_player_dissolve->stop();
	anim_player_respawn->play("Respawn");
}
void Crystal::_on_AnimationPlayerDissolve_animation_finished(String anim_name)
{
	/*if (anim_name == "Dissolve")
		queue_free();*/
}
