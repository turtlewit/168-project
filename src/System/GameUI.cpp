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
	REGISTER_METHOD(GameUI, reset_pounce_bar);
}

CLASS_INITS(GameUI)


void GameUI::_ready()
{
	for (int i = 0; i < 4; i++) {
		crystal_labels[i] = cast_to<Label>(get_node("Crystals")->get_child(i)->get_child(0));
		update_crystal_amount(i, 0);
	}
	health_amount = cast_to<Label>(get_node("HealthBar")->get_child(0));
	health_bar = GET_NODE(ProgressBar, "HealthBar");
	pounce_bar = GET_NODE(ProgressBar, "PounceBar");
	tween_health = GET_NODE(Tween, "TweenHealth");
	tween_pounce = GET_NODE(Tween, "TweenPounce");
}


void GameUI::_enter_tree()
{
	SignalManagerPlayer* manager = SignalManagerPlayer::get_singleton();
	manager->connect("player_crystal_amount_changed", this, NAMEOF(update_crystal_amount));
	manager->connect("player_damaged", this, NAMEOF(update_health));
	manager->connect("player_pounced", this, NAMEOF(reset_pounce_bar));
}

void GameUI::_exit_tree()
{
	SignalManagerPlayer* manager = SignalManagerPlayer::get_singleton();
	manager->disconnect("player_crystal_amount_changed", this, NAMEOF(update_crystal_amount));
	manager->disconnect("player_damaged", this, NAMEOF(update_health));
	manager->disconnect("player_pounced", this, NAMEOF(reset_pounce_bar));
}


void GameUI::update_crystal_amount(int index, int amount)
{
	crystal_amounts[index] += amount;
	crystal_labels[index]->set_text(String::num_int64(crystal_amounts[index]));
}


void GameUI::update_health(int old_health, int damage)
{
	tween_health->interpolate_property(health_bar, "value", old_health, old_health - damage, 1.0f, Tween::TRANS_QUINT, Tween::EASE_OUT);
	tween_health->start();
	health_amount->set_text(String::num_int64(damage <= old_health ? old_health - damage : 0));
}


void GameUI::reset_pounce_bar()
{
	pounce_bar->set_value(0);
	tween_pounce->interpolate_property(pounce_bar, "value", 0, 3, 3.0f, Tween::TRANS_LINEAR);
	tween_pounce->start();
}
