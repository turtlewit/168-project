#include <Array.hpp>
#include <NetworkedMultiplayerENet.hpp>

#include "Utils/Defs.hpp"
#include "Net/NetworkManager.hpp"

using namespace godot;

NetworkManager* NetworkManager::singleton = nullptr;

void NetworkManager::start_host()
{
    check_connection_info();
}

void NetworkManager::start_server()
{
    check_connection_info();

    if (max_clients <= 0)
        PRINT_ERROR(String("Invalid number of max clients! %d").format(Array::make(max_clients)));

    Ref<NetworkedMultiplayerENet> server_peer = Ref(NetworkedMultiplayerENet::_new());
    server_peer.create_server(port, max_clients);
    get_tree().set_network_peer(server_peer);
    set_network_master(SERVER_ID);

    call_deferred("emit_signal", "network_connected");
}

void NetworkManager::start_client()
{
    check_connection_info();
}

void NetworkManager::_register_methods()
{
    register_signal<NetworkManager>("network_connected");

    register_property<NetworkManager, String>("address", &NetworkManager::set_address, &NetworkManager::get_address, "localhost");
    register_property<NetworkManager, int64_t>("port", &NetworkManager::set_port, &NetworkManager::get_port, 0);
    register_property<NetworkManager, int64_t>("max_clients", &NetworkManager::set_max_clients, &NetworkManager::get_max_clients, 0);
}

void NetworkManager::_init()
{
}

NetworkManager::NetworkManager()
    : address{""}, port{0}, max_clients{0}
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

    PRINT_ERROR(String("Invalid connection information! %s %d").format(Array::make(address, port)), "NetworkManager::check_connection_info");
}

void NetworkManager::_on_network_peer_connected(int64_t master_id)
{
    
}