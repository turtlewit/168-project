#include "Portal.hpp"
#include "Utils/Defs.hpp"
#include "Player/Player.hpp"


using namespace godot;


void Portal::_register_methods()
{
	
	REGISTER_METHOD(Portal, _ready);
	REGISTER_METHOD(Portal, _on_Portal_body_entered);
}
CLASS_INITS(Portal)

void Portal::_ready()
{

}

void Portal::_on_Portal_body_entered(Node* body) {

}