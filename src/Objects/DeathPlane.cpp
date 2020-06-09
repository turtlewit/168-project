// DeathPlane.cpp
#include "DeathPlane.hpp"

#include "Utils/Defs.hpp"
#include "Utils/Mathf.hpp"
#include "Player/Player.hpp"
#include "System/SignalManagerPlayer.hpp"


using namespace godot;

void DeathPlane::_register_methods()
{

	REGISTER_METHOD(DeathPlane, _on_DeathPlane_body_entered);
}

CLASS_INITS(DeathPlane)
void DeathPlane::_ready()
{

}
void DeathPlane::_on_DeathPlane_body_entered(Node* body)
{
	if (body->is_in_group("Player")) {
		Player* player = cast_to<Player>(body);
		player->translate(Vector3(0, 40, 0));
		player->damage(1);
		player->set_gravity_velocity(0.0);
	}
}