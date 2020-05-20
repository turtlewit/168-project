// GameUI.hpp
#pragma once

#include <Godot.hpp>
#include <Control.hpp>
#include <Label.hpp>
#include <ProgressBar.hpp>
#include <Tween.hpp>

namespace godot {

	class GameUI : public Control {
		GODOT_CLASS(GameUI, Control)

	private:
		unsigned int crystal_amounts[4] = {0};
		Label* crystal_labels[4];

		ProgressBar* health_bar;
		ProgressBar* pounce_bar;
		Tween* tween_health;
		Tween* tween_pounce;

	public:
		static void _register_methods();

		GameUI();
		~GameUI();
		void _init();

		void _ready();
		void _enter_tree();
		void _exit_tree();

		void update_crystal_amount(int index, int amount);
		void update_health(int old_health, int amount);
		void reset_pounce_bar();
	};

}
