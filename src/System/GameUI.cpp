// GameUI.cpp
#include <ProjectSettings.hpp>
#include <OS.hpp>
#include <InputEventKey.hpp>

#include "GameUI.hpp"
#include "System/SignalManagerPlayer.hpp"
#include "Utils/Defs.hpp"
#include "Net/Utils.hpp"
#include "GameManager.hpp"


using namespace godot;

namespace {
	String get_button_name() {
		if (!ProjectSettings::get_singleton()->has_setting("input/start_round"))
			return "Enter";
		
		Array mappings = static_cast<Dictionary>(ProjectSettings::get_singleton()->get_setting("input/start_round"))["events"];
		for (int i = 0; i < mappings.size(); ++i) {
			Ref<InputEventKey> key = mappings[i];
			if (key == Ref<InputEventKey>() || !IS_CLASS(key, InputEventKey))
				continue;
			return OS::get_singleton()->get_scancode_string(key->get_scancode());
		}
		return "Enter";
	}
}

void GameUI::_register_methods()
{
	REGISTER_METHOD(GameUI, _ready);
	REGISTER_METHOD(GameUI, _enter_tree);
	REGISTER_METHOD(GameUI, _exit_tree);
	REGISTER_METHOD(GameUI, update_crystal_amount);
	REGISTER_METHOD(GameUI, update_health);
	REGISTER_METHOD(GameUI, reset_pounce_bar);
	REGISTER_METHOD(GameUI, _on_game_controller_state_changed);
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
	GameManager::get_singleton()->connect("state_changed", this, "_on_game_controller_state_changed");
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


void GameUI::_on_game_controller_state_changed(int state)
{
	if (!IS_SERVER)
		return;

	switch(static_cast<GameManager::State>(state)) {
		case GameManager::State::lobby: {
			Label* start_round_label = cast_to<Label>(get_node("HostStartRound"));
			start_round_label->set_text(start_round_label->get_text().format(Dictionary::make("button", get_button_name())));
			start_round_label->set_visible(true);
		} break;
		default: {
			Label* start_round_label = cast_to<Label>(get_node("HostStartRound"));
			start_round_label->set_visible(false);
		} break;
	}
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
