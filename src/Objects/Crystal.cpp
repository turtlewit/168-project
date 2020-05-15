// Crystal.cpp
#include "Crystal.hpp"

#include "Utils/Defs.hpp"
#include "Player/Player.hpp"
#include "System/SignalManagerPlayer.hpp"

using namespace godot;

void Crystal::_register_methods()
{
	REGISTER_METHOD(Crystal, _ready);
	REGISTER_METHOD(Crystal, _on_Crystal_body_entered);
	REGISTER_METHOD(Crystal, _on_AnimationPlayerDissolve_animation_finished);

	REGISTER_PROPERTY_HINT(Crystal, int, powerup_type, 0, GODOT_PROPERTY_HINT_ENUM, "Speed,Jump,Pounce Range,Damage");

	register_signal<Crystal>("picked_up", Dictionary{});
}

CLASS_INITS(Crystal)


void Crystal::_ready()
{
	anim_player_dissolve = GET_NODE(AnimationPlayer, "AnimationPlayerDissolve");

	powerup_type_internal = static_cast<Powerup>(powerup_type);
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

		SignalManagerPlayer::get_singleton()->emit_signal("player_crystal_amount_changed", static_cast<int>(powerup_type_internal), 1);
	}
}


void Crystal::_on_AnimationPlayerDissolve_animation_finished(String anim_name)
{
	if (anim_name == "Dissolve")
		queue_free();
}
