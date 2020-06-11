#include "Portal.hpp"
#include "Utils/Defs.hpp"
#include "Player/Player.hpp"
#include "Utils/Mathf.hpp"

#include <AudioStreamPlayer3D.hpp>

using namespace godot;


void Portal::_register_methods()
{
	
	REGISTER_METHOD(Portal, _ready);
	REGISTER_METHOD(Portal, _on_Portal_body_entered);

	register_property("teleport location", &Portal::translation, Vector3(0,0,0));
}
CLASS_INITS(Portal)

void Portal::_ready()
{

}

void Portal::_on_Portal_body_entered(Node* body) {
	if (body->is_in_group("Player"))
	{
		GET_NODE(AudioStreamPlayer3D, "SoundTeleport")->play();
		Player* p = cast_to<Player>(body);
		p->set_translation(cast_to<Position3D>(get_child(0))->get_global_transform().origin);
	}
}