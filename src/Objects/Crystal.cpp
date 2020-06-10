// Crystal.cpp
#include "Crystal.hpp"

#include "Utils/Defs.hpp"
#include "Utils/Mathf.hpp"
#include "Player/Player.hpp"
#include "System/SignalManagerPlayer.hpp"

#include <Timer.hpp>
#include <OmniLight.hpp>
#include <Animation.hpp>

using namespace godot;

void Crystal::_register_methods()
{
	REGISTER_METHOD(Crystal, _ready);

	REGISTER_METHOD(Crystal, respawn);

	REGISTER_METHOD(Crystal, _on_Crystal_body_entered);

	REGISTER_PROPERTY_HINT(Crystal, int, powerup_type, 0, GODOT_PROPERTY_HINT_ENUM, "Speed,Jump,Pounce Range,Damage");

	register_property("respawn_time", &Crystal::respawn_time, 10.0f);
	register_signal<Crystal>("picked_up", Dictionary{});
}

CLASS_INITS(Crystal)


void Crystal::_ready()
{
	anim_player_dissolve = GET_NODE(AnimationPlayer, "AnimationPlayerDissolve");
	anim_player_respawn = GET_NODE(AnimationPlayer, "AnimationPlayerRespawn");
	powerup_type_internal = static_cast<Powerup>(powerup_type);
	GET_NODE(Timer, "Timer")->set_wait_time(respawn_time);

	set_color();
}


void Crystal::_on_Crystal_body_entered(Node* body)
{
	if (!collected && body->is_in_group("Player")) {
		anim_player_dissolve->play("Dissolve");
		collected = true;
		
		Player* player = cast_to<Player>(body);
		switch (powerup_type_internal) {
			case Powerup::Speed:
				player->increase_speed();
				break;
			case Powerup::Jump:
				player->increase_jump();
				break;
			case Powerup::PounceRange:
				player->increase_pounce();
				break;
			case Powerup::Damage: {
				player->increase_swipe_damage();
				player->increase_pounce_damage();
			} break;
		}

		GET_NODE(Timer, "Timer")->start();
		SignalManagerPlayer::get_singleton()->emit_signal("player_crystal_amount_changed", static_cast<int>(powerup_type_internal), 1);
	}
}

void Crystal::respawn()
{
	collected = false;
	powerup_type_internal = static_cast<Powerup>(Mathf::rand_range(0, 3));
	set_color();

	anim_player_respawn->play("Respawn");
}

void Crystal::set_color()
{
	Ref<ShaderMaterial> mat = GET_NODE(MeshInstance, "Model/Cube")->get_surface_material(0);
	uint32_t color = 0x00c0ffff;

	switch (powerup_type_internal) {
	case Powerup::Speed:
		color = 0x00c0ffff;
		break;
	case Powerup::Jump:
		color = 0xc000ffff;
		break;
	case Powerup::PounceRange:
		color = 0xfffc00ff;
		break;
	case Powerup::Damage:
		color = 0xff6c00ff;
		break;
	}

	mat->set_shader_param("colorBlend", Color::hex(color));
	anim_player_dissolve->get_animation("Dissolve")->track_set_path(0, "Model/Cube:material/0:shader_param/amount");
	anim_player_respawn->get_animation("Respawn")->track_set_path(0, "Model/Cube:material/0:shader_param/amount");

	GET_NODE(MeshInstance, "Model/Cube")->set_surface_material(0, mat->duplicate());
	GET_NODE(OmniLight, "Model/OmniLight")->set_color(Color::hex(color));
}
