#include "Portal.hpp"
#include "Utils/Defs.hpp"
#include "Player/Player.hpp"


using namespace godot;


void Portal::_register_methods()
{
	
	REGISTER_METHOD(Portal, _ready);
	REGISTER_METHOD(Portal, _on_Portal_body_entered);

	register_property("teleport location", &Portal::translation, Vector3(0,0,0));
	register_property("teleport rotation", &Portal::rotation, Vector3(0,0,0));
}
CLASS_INITS(Portal)

void Portal::_ready()
{

}

void Portal::_on_Portal_body_entered(Node* body) {
	Player* other;
	if (body->is_in_group("Player"))
	{
		other = cast_to<Player>(body->get_parent());
		Godot::print("PLAYER TOOK PORTAL!!!");
		/*other->set_translation(translation);
		other->set_rotation(rotation);*/
	}
}