#include <Godot.hpp>
#include <SceneTree.hpp>

#include "Net/NetworkManager.hpp"

#define IS_SERVER (get_tree()->get_network_unique_id() == NetworkManager::SERVER_ID)
#define IS_MASTER (get_network_master() == get_tree()->get_network_unique_id())
