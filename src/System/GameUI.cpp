// GameUI.cpp
#include "GameUI.hpp"

#include "System/SignalManagerPlayer.hpp"
#include "Utils/Defs.hpp"

using namespace godot;

void GameUI::_register_methods()
{

}

CLASS_INITS(GameUI)


void GameUI::_enter_tree()
{
	SignalManagerPlayer::get_singleton()->connect("player_crystal_amount_changed", this, NAMEOF(update_crystal_amount));
}

void GameUI::_exit_tree()
{
	SignalManagerPlayer::get_singleton()->disconnect("player_crystal_amount_changed", this, NAMEOF(update_crystal_amount));
}


void GameUI::update_crystal_amount(int amount, int index)
{

}
