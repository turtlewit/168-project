#include "Portal.hpp"
#include "Utils/Defs.hpp"
#include "Player/Player.hpp"
#include "Utils/Mathf.hpp"

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
		Player* p = cast_to<Player>(body);
		Transform t;
		t.translate(translation);
		p->set_global_transform(t);
	}
}