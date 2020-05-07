// GameUI.hpp
#pragma once

#include <Godot.hpp>
#include <Control.hpp>
#include <Label.hpp>

namespace godot {

	class GameUI : public Control {
		GODOT_CLASS(GameUI, Control)

	private:
		unsigned int crystal_amounts[5] = {0};
		Label* crystal_labels[5];

	public:
		static void _register_methods();

		GameUI();
		~GameUI();
		void _init();

		void _ready();
		void _enter_tree();
		void _exit_tree();

		void update_crystal_amount(int index, int amount);
	};

}
