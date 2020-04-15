#pragma once

#include <Godot.hpp>
#include <Node.hpp>
#include <SceneTree.hpp>

#include <Net/NetworkManager.hpp>
#include <Net/NetworkIdentity.hpp>
#include <Utils/Defs.hpp>

template <typename T, typename C>
class NetworkNode : public T {
public:
    static void _register_methods();

    bool is_network_connected();
    bool is_master();
    bool is_puppet();
    bool is_server();
    bool is_client();

    virtual void _on_network_start();
    void _ready();

    virtual ~NetworkNode() {}

protected:
    void _on_network_node_start();
};

template <typename T, typename C>
void NetworkNode<T, C>::_register_methods()
{
    godot::register_method<void (C::*)()>("_on_network_node_start", reinterpret_cast<void (C::*)()>(&NetworkNode::_on_network_node_start));
    godot::register_method<void (C::*)()>("_ready", &C::_ready);
}

template <typename T, typename C>
void NetworkNode<T, C>::_on_network_start()
{
}

template <typename T, typename C>
void NetworkNode<T, C>::_on_network_node_start()
{
    _on_network_start();
}

template <typename T, typename C>
void NetworkNode<T, C>::_ready()
{
    T::set_process(false);

    godot::Node* identity_node = nullptr;
    NetworkIdentity* identity = nullptr;

    identity_node = T::get_node(godot::NodePath("../NetworkIdentity"));

    if (identity_node == nullptr) {
        godot::Array children = T::get_parent()->get_children();
        for (int i = 0; i < children.size(); ++i) {
            identity = this->template cast_to<NetworkIdentity>(children[i]);

            if (identity != nullptr)
                break;
        }
    } else {
        identity = this->template cast_to<NetworkIdentity>(identity_node);
    }

    if (identity == nullptr) {
        PRINT_ERROR("NetworkIdentity node not found!", "NetworkNode::_ready()");
        return;
    }

    identity->connect("network_node_start", this, "_on_network_node_start", godot::Array(), T::CONNECT_REFERENCE_COUNTED);
}

template <typename T, typename C>
bool NetworkNode<T, C>::is_network_connected()
{
    return T::get_tree()->has_network_peer();
}

template <typename T, typename C>
bool NetworkNode<T, C>::is_master()
{
    return (T::get_network_master() == 0 || T::is_network_master());
}

template <typename T, typename C>
bool NetworkNode<T, C>::is_puppet()
{
    return !is_master();
}

template <typename T, typename C>
bool NetworkNode<T, C>::is_server()
{
    return T::get_tree()->get_network_unique_id() == NetworkManager::SERVER_ID;
}

template <typename T, typename C>
bool NetworkNode<T, C>::is_client()
{
    return (NetworkManager::get_singleton()->is_host() && is_server()) || !is_server();
}

