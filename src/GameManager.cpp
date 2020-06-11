#include <Timer.hpp>

#include "GameManager.hpp"
#include "Net/Utils.hpp"
#include "Utils/Defs.hpp"
#include "Player/Player.hpp"

using namespace godot;

namespace {
	constexpr const char* state_name(GameManager::State state) {
		switch(state) {
			case GameManager::State::disconnected: return "disconnected";
			case GameManager::State::lobby: return "lobby";
			case GameManager::State::collection: return "collection";
			case GameManager::State::arena: return "arena";
			case GameManager::State::end: return "end";
		}
		return "";
	}
}

GameManager* GameManager::singleton = nullptr;

void GameManager::_register_methods()
{
	register_signal<GameManager>("state_changed", Dictionary::make("state", Variant::Type::INT));

	register_method("_ready", &GameManager::_ready);
	register_method("_on_network_connected", &GameManager::_on_network_connected);
	register_method("_on_server_disconnected", &GameManager::_on_server_disconnected);
	register_method("change_state", reinterpret_cast<void(GameManager::*)(int)>(&GameManager::change_state), GODOT_METHOD_RPC_MODE_PUPPETSYNC);
	register_method("_on_collection_phase_timer_timout", &GameManager::_on_collection_phase_timer_timout);
	register_method("_on_win_timer_timout", &GameManager::_on_win_timer_timout);
	register_method("decrement_player_count", &GameManager::decrement_player_count);

	register_property("state", reinterpret_cast<int(GameManager::*)>(&GameManager::state), static_cast<int>(State::disconnected), GODOT_METHOD_RPC_MODE_DISABLED, static_cast<godot_property_usage_flags>(0) /* don't store and don't show in inspector */);
	register_property("winner", &GameManager::winner, String(), GODOT_METHOD_RPC_MODE_REMOTE);
}

void GameManager::_init()
{
	singleton = this;
}

void GameManager::_ready()
{
	get_tree()->connect("server_disconnected", this, "_on_server_disconnected");
}

void GameManager::_on_network_connected()
{
	set_network_master(NetworkManager::SERVER_ID);

	change_state(State::lobby);

	if (IS_MASTER) {
		get_node("CollectionPhaseTimer")->connect("timeout", this, "_on_collection_phase_timer_timout");
		get_node("WinTimer")->connect("timeout", this, "_on_win_timer_timout");
	}
}

void GameManager::_on_server_disconnected()
{
	get_tree()->change_scene("res://Scenes/MainWorldKat.tscn");
}

void GameManager::change_state(State to)
{
	state = to;
	Godot::print("Changing State: {0}", state_name(to));
	emit_signal("state_changed", static_cast<int>(to));

	switch (state) {
		case State::collection:
			if (IS_MASTER) {
				Timer* timer = cast_to<Timer>(get_node("CollectionPhaseTimer"));
				timer->start();
			}
			break;
		case State::arena:
			arena_start();
			break;
		default: break;
	}
}

void GameManager::_on_collection_phase_timer_timout()
{
	rpc("change_state", static_cast<int>(State::arena));
}

void GameManager::_on_win_timer_timout()
{
	rpc("change_state", static_cast<int>(State::lobby));
}

void GameManager::arena_start()
{
	if (!IS_SERVER)
		return;

	Array players = get_tree()->get_nodes_in_group("Player");
	players_left = 0;
	for (int i = 0; i < players.size(); ++i)
	{
		Player* player = cast_to<Player>(players[i]);

		if (!player)		
			continue;
                            		
		++players_left;

		if (!player->is_connected("player_arena_died", this, "decrement_player_count"))
			player->connect("player_arena_died", this, "decrement_player_count");
		if (!player->is_connected("tree_exiting", this, "decrement_player_count"))
			player->connect("tree_exiting", this, "decrement_player_count");
	}

	if (players_left == 1) {
		win();
	}
}

void GameManager::decrement_player_count()
{
	--players_left;
	if (players_left == 1) {
		win();
	}
}

void GameManager::win()
{
	Array players = get_tree()->get_nodes_in_group("Player");
	winner = "Nobody"; // :c
	for (int i = 0; i < players.size(); ++i)
	{
		Player* player = cast_to<Player>(players[i]);
		if (player && !player->is_dead()) {
			winner = player->get_name();
			break;
		}
	}

	Timer* timer = cast_to<Timer>(get_node("WinTimer"));
	timer->start();
	rset("winner", winner);
	rpc("change_state", static_cast<int>(State::end));
}
