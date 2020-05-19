// GameUI.cpp
#include "GameUI.hpp"

#include "System/SignalManagerPlayer.hpp"
#include "Utils/Defs.hpp"

using namespace godot;

void GameUI::_register_methods()
{
	REGISTER_METHOD(GameUI, _ready);
	REGISTER_METHOD(GameUI, _enter_tree);
	REGISTER_METHOD(GameUI, _exit_tree);
	REGISTER_METHOD(GameUI, update_crystal_amount);
}

CLASS_INITS(GameUI)


void GameUI::_ready()
{
	for (int i = 0; i < 4; i++) {
		crystal_labels[i] = cast_to<Label>(get_node("Crystals")->get_child(i)->get_child(0));
		update_crystal_amount(i, 0);
	}
}


void GameUI::_enter_tree()
{
	SignalManagerPlayer::get_singleton()->connect("player_crystal_amount_changed", this, NAMEOF(update_crystal_amount));
}

void GameUI::_exit_tree()
{
	SignalManagerPlayer::get_singleton()->disconnect("player_crystal_amount_changed", this, NAMEOF(update_crystal_amount));
}


void GameUI::update_crystal_amount(int index, int amount)
{
	crystal_amounts[index] += amount;
	crystal_labels[index]->set_text(String::num_int64(crystal_amounts[index]));
}
