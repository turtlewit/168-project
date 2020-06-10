#include <Array.hpp>
#include <NetworkedMultiplayerENet.hpp>
#include <SceneTree.hpp>
#include <Spatial.hpp>

#include "Utils/Defs.hpp"
#include "Net/NetworkManager.hpp"
#include "Utils/Mathf.hpp"
using namespace godot;

NetworkManager* NetworkManager::singleton = nullptr;

void NetworkManager::_register_methods()
{
    register_signal<NetworkManager>("network_connected", Dictionary());

    register_property<NetworkManager, String>("address", &NetworkManager::set_address, &NetworkManager::get_address, "localhost");
    register_property<NetworkManager, int64_t>("port", &NetworkManager::set_port, &NetworkManager::get_port, 0);
    register_property<NetworkManager, int64_t>("max_clients", &NetworkManager::set_max_clients, &NetworkManager::get_max_clients, 0);

	register_property("player_prefab", &NetworkManager::player_prefab, Ref<PackedScene>());

    register_method("_enter_tree", &NetworkManager::_enter_tree);
    register_method("_on_network_peer_connected", &NetworkManager::_on_network_peer_connected);

    register_method("start_host", &NetworkManager::start_host);
    register_method("start_server", &NetworkManager::start_server);
    register_method("start_client", &NetworkManager::start_client);

    register_method("spawn_player_with_master", &NetworkManager::spawn_player_with_master, GODOT_METHOD_RPC_MODE_REMOTE);
}

void NetworkManager::start_host()
{
    host = true;
    start_server();
    call_deferred("spawn_player_with_master", SERVER_ID);
}

void NetworkManager::start_server()
{
    check_connection_info();

    if (max_clients <= 0)
        PRINT_ERROR(String("Invalid number of max clients! {0}").format(Array::make(max_clients)), "NetworkManager::start_server()");

    Ref<NetworkedMultiplayerENet> server_peer = Ref(NetworkedMultiplayerENet::_new());
    server_peer->create_server(port, max_clients);
    get_tree()->set_network_peer(server_peer);
    set_network_master(SERVER_ID);

    call_deferred("emit_signal", "network_connected");
}

void NetworkManager::start_client()
{
    check_connection_info();

    Ref<NetworkedMultiplayerENet> server_peer = Ref(NetworkedMultiplayerENet::_new());
    if (server_peer->create_client(address, port) != Error::OK) {
        PRINT_ERROR(String("Could not connect to server! Address: \"{0}\" Port: {1}").format(Array::make(address, port)), "NetworkManager::start_client");
        return;
    }

    get_tree()->set_network_peer(server_peer);
}

void NetworkManager::spawn_player_with_master(int64_t master_id)
{
    if (player_prefab == Ref<PackedScene>())
        return;

    godot::Node* player = player_prefab->instance();

    // Append the master ID to the player's name.
    // This is because Godot knows two nodes are the same across the network
    // based on their node path and name. 
    player->set_name(String("{0}{1}").format(Array::make(player->get_name(), master_id)));

    get_tree()->get_current_scene()->add_child(player);

    Node* spawn_points = get_tree()->get_current_scene()->find_node(String("SpawnPoints"), true, false);
    if (spawn_points) {
        Spatial* spatial_player = cast_to<Spatial>(player);
        Transform player_transform = spatial_player->get_global_transform();
        player_transform.origin = cast_to<Spatial>(spawn_points->get_children()[Mathf::rand_range(0, 6)])->get_global_transform().origin;
        spatial_player->set_global_transform(player_transform);
    }

    player->set_network_master(master_id);
}

void NetworkManager::_init()
{
}

void NetworkManager::_enter_tree()
{
    get_tree()->connect("network_peer_connected", this, "_on_network_peer_connected", Array());
}

NetworkManager::NetworkManager()
    : address{ "localhost" }, port{ 0 }, max_clients{ 0 }, host{ false }
{
    if (singleton != nullptr)
        PRINT_ERROR("NetworkManager constructed twice!", "NetworkManager::NetworkManager");

    singleton = this;
}

NetworkManager::~NetworkManager()
{
    if (singleton == this)
        singleton = nullptr;
}

void NetworkManager::check_connection_info()
{
    bool valid = true;

    if (address == "")
        valid = false;

    if (port <= 0)
        valid = false;

    if (valid)
        return;

    PRINT_ERROR(String("Invalid connection information! address: {0} port: {1}").format(Array::make(address, port)), "NetworkManager::check_connection_info");
}

void NetworkManager::_on_network_peer_connected(int64_t master_id)
{
    if (master_id == SERVER_ID) {
        set_network_master(master_id);
        emit_signal("network_connected");
        int64_t my_id = get_tree()->get_network_unique_id();

        spawn_player_with_master(my_id);
        rpc("spawn_player_with_master", my_id);
    }

    Godot::print("Connected to peer {0}", master_id);
}
