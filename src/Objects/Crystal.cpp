// Crystal.cpp
#include "Crystal.hpp"

#include "Utils/Defs.hpp"
#include "Player/Player.hpp"

using namespace godot;

void Crystal::_register_methods()
{
	REGISTER_METHOD(Crystal, _ready);
	REGISTER_METHOD(Crystal, _on_Crystal_body_entered);
	REGISTER_METHOD(Crystal, _on_AnimationPlayerDissolve_animation_finished);

	REGISTER_PROPERTY_HINT(Crystal, int, powerup_type, 0, GODOT_PROPERTY_HINT_ENUM, "Speed,Jump,Pounce Range,Swipe Damage,Pounce Damage");

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
			case Powerup::SwipeDamage:
				player->increase_swipe_damage(1);
				break;
			case Powerup::PounceDamage:
				player->increase_pounce_damage(1);
				break;
		}
	}
}


void Crystal::_on_AnimationPlayerDissolve_animation_finished(String anim_name)
{
	if (anim_name == "Dissolve")
		queue_free();
}
