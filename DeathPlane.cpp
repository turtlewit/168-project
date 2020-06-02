// DeathPlane.cpp
#include "DeathPlane.hpp"

#include "Utils/Defs.hpp"
#include "Utils/Mathf.hpp"
#include "Player/Player.hpp"
#include "System/SignalManagerPlayer.hpp"

#include <Timer.hpp>
#include <OmniLight.hpp>
#include <Animation.hpp>

using namespace godot;

void DeathPlane::_register_methods()
{

	REGISTER_METHOD(DeathPlane, _on_Deathplane_body_entered);
}

CLASS_INITS(DeathPlane)

void DeathPlane::_on_Deathplane_body_entered(Node* body)
{
	if (body->is_in_group("Player")) {
		Player* player = cast_to<Player>(body);
		player->translate(Vector3(player->get_translation().x, player->get_translation().y+10, player->get_translation().z));
	}
}