#pragma once

#include <Godot.hpp>
#include <Node.hpp>
#include <String.hpp>
#include <PackedScene.hpp>

// NetworkManager
//
// Signals:
//   network_connected()
//     Called when a server is started or when a server has been connected to.
//     Use this for network initialization.
class NetworkManager : public godot::Node {
    GODOT_CLASS(NetworkManager, godot::Node)

public:
    static NetworkManager* get_singleton() { return singleton; }
    static void _register_methods();
    static const int64_t SERVER_ID = 1;

    godot::String get_address() { return address; }
    void set_address(godot::String address) { this->address = address; }

    int64_t get_port() { return port; }
    void set_port(int64_t port) { this->port = port; }

    int64_t get_max_clients() { return max_clients; }
    void set_max_clients(int64_t max_clients) { this->max_clients = max_clients; }

    bool is_host() { return host; }

    void start_host();
    void start_server();
    void start_client();

    void spawn_player_with_master(int64_t master_id);

    void _init();
    void _enter_tree();

    NetworkManager();
    ~NetworkManager();

private:
    void check_connection_info();
    void _on_network_peer_connected(int64_t master_id);

private:
    static NetworkManager* singleton;

    godot::String address;
    int64_t port;
    int64_t max_clients;
    bool host;

    godot::Ref<godot::PackedScene> player_prefab;
};