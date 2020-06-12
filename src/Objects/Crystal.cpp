// Crystal.cpp
#include "Crystal.hpp"

#include "Utils/Defs.hpp"
#include "Utils/Mathf.hpp"
#include "Player/Player.hpp"
#include "System/SignalManagerPlayer.hpp"
#include "GameManager.hpp"

#include <Timer.hpp>
#include <OmniLight.hpp>
#include <Animation.hpp>
#include <SceneTree.hpp>

using namespace godot;

void Crystal::_register_methods()
{
	REGISTER_METHOD(Crystal, _ready);

	REGISTER_METHOD(Crystal, respawn);

	REGISTER_METHOD(Crystal, _on_Crystal_body_entered);

	REGISTER_METHOD(Crystal, _on_game_manager_state_changed);

	REGISTER_PROPERTY_HINT(Crystal, int, powerup_type, 0, GODOT_PROPERTY_HINT_ENUM, "Speed,Jump,Swipe,Pounce");

	register_property("respawn_time", &Crystal::respawn_time, 10.0f);
	register_signal<Crystal>("picked_up", Dictionary{});
}

CLASS_INITS(Crystal)


void Crystal::_ready()
{
	anim_player_dissolve = GET_NODE(AnimationPlayer, "AnimationPlayerDissolve");
	anim_player_respawn = GET_NODE(AnimationPlayer, "AnimationPlayerRespawn");
	powerup_type_internal = static_cast<Powerup>(Mathf::rand_range(0,3));
	GET_NODE(Timer, "Timer")->set_wait_time(respawn_time);

	set_color();
	
	GameManager::get_singleton()->connect("state_changed", this, "_on_game_manager_state_changed");
}


void Crystal::_on_Crystal_body_entered(Node* body)
{
	if (!collected && body->is_in_group("Player")) {
		if (body->get_network_master() != get_tree()->get_network_unique_id())
			return;

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
			case Powerup::Swipe:
				player->increase_swipe();
				break;
			case Powerup::Pounce: {
				player->increase_pounce();
			} break;
		}

		GET_NODE(AudioStreamPlayer3D, "SoundPickup")->play();

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
	case Powerup::Swipe:
		color = 0xfffc00ff;
		break;
	case Powerup::Pounce:
		color = 0xff6c00ff;
		break;
	}

	mat->set_shader_param("colorBlend", Color::hex(color));
	anim_player_dissolve->get_animation("Dissolve")->track_set_path(0, "Model/Cube:material/0:shader_param/amount");
	anim_player_respawn->get_animation("Respawn")->track_set_path(0, "Model/Cube:material/0:shader_param/amount");

	GET_NODE(MeshInstance, "Model/Cube")->set_surface_material(0, mat->duplicate());
	GET_NODE(OmniLight, "Model/OmniLight")->set_color(Color::hex(color));
}

void Crystal::_on_game_manager_state_changed(int state) {
	respawn();
}

