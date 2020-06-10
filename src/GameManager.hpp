#pragma once

#include <Godot.hpp>
#include <Node.hpp>

class GameManager : public godot::Node {
	GODOT_CLASS(GameManager, godot::Node)

public:
	enum class State {
		disconnected,
		lobby,
		collection,
		arena,
		end
	};

	static void _register_methods();
	static GameManager* get_singleton() { return singleton; }

	void _init();
	void _ready();
	void _on_network_connected();
	void _on_server_disconnected();

	State get_state() { return state; }

private:
	void change_state(State to);

	static GameManager* singleton;

	State state = State::disconnected;
};
