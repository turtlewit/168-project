// SignalManagerPlayer.cpp
#include "SignalManagerPlayer.hpp"

#include "Utils/Defs.hpp"

using namespace godot;

SignalManagerPlayer* SignalManagerPlayer::singleton = nullptr;

void SignalManagerPlayer::_register_methods()
{
	register_signal<SignalManagerPlayer>("player_damaged", "old_health", GODOT_VARIANT_TYPE_INT, "amount", GODOT_VARIANT_TYPE_INT, "player", GODOT_VARIANT_TYPE_STRING);
	register_signal<SignalManagerPlayer>("player_crystal_amount_changed", "index", GODOT_VARIANT_TYPE_INT, "amount_added", GODOT_VARIANT_TYPE_INT);
	register_signal<SignalManagerPlayer>("player_pounced", Dictionary{});
	register_signal<SignalManagerPlayer>("player_connected", GODOT_VARIANT_TYPE_STRING, "username");
}


SignalManagerPlayer::SignalManagerPlayer()
{
	singleton = this;
}

SignalManagerPlayer::~SignalManagerPlayer() {}

void SignalManagerPlayer::_init() {}


SignalManagerPlayer* SignalManagerPlayer::get_singleton()
{
	return singleton;
}
