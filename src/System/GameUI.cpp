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
	REGISTER_METHOD(GameUI, update_health);
}

CLASS_INITS(GameUI)


void GameUI::_ready()
{
	for (int i = 0; i < 4; i++) {
		crystal_labels[i] = cast_to<Label>(get_node("Crystals")->get_child(i)->get_child(0));
		update_crystal_amount(i, 0);
	}

	health_bar = GET_NODE(ProgressBar, "HealthBar");
	tween = GET_NODE(Tween, "Tween");
}


void GameUI::_enter_tree()
{
	SignalManagerPlayer::get_singleton()->connect("player_crystal_amount_changed", this, NAMEOF(update_crystal_amount));
	SignalManagerPlayer::get_singleton()->connect("player_damaged", this, NAMEOF(update_health));
}

void GameUI::_exit_tree()
{
	SignalManagerPlayer::get_singleton()->disconnect("player_crystal_amount_changed", this, NAMEOF(update_crystal_amount));
	SignalManagerPlayer::get_singleton()->disconnect("player_damaged", this, NAMEOF(update_health));
}


void GameUI::update_crystal_amount(int index, int amount)
{
	crystal_amounts[index] += amount;
	crystal_labels[index]->set_text(String::num_int64(crystal_amounts[index]));
}


void GameUI::update_health(int old_health, int damage)
{
	tween->interpolate_property(health_bar, "value", old_health, old_health - damage, 1.0f, Tween::TRANS_QUINT, Tween::EASE_OUT);
	tween->start();
}
