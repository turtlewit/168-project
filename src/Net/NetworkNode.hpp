#pragma once

#include <Godot.hpp>
#include <Node.hpp>
#include <SceneTree.hpp>

#include <Net/NetworkManager.hpp>

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
    void __on_network_start();
};

template <typename T, typename C>
void NetworkNode<T, C>::_register_methods()
{
    godot::register_method<void (C::*)()>("__on_network_start", &NetworkNode::__on_network_start);
    godot::register_method<void (C::*)()>("_ready", &NetworkNode::_ready);
}

template <typename T, typename C>
void NetworkNode<T, C>::_on_network_start()
{
}

template <typename T, typename C>
void NetworkNode<T, C>::__on_network_start()
{
    _on_network_start();
}

template <typename T, typename C>
void NetworkNode<T, C>::_ready()
{
    if (is_network_connected()) {
        _on_network_start();
        return;
    }

    T::set_process(false);

    NetworkManager::get_singleton()->connect("network_connected", this, "__on_network_start", godot::Array(), T::CONNECT_REFERENCE_COUNTED);
}

template <typename T, typename C>
bool NetworkNode<T, C>::is_network_connected()
{
    return T::get_tree()->has_network_peer();
}

template <typename T, typename C>
bool NetworkNode<T, C>::is_master()
{
    return (T::get_network_master() == 0 || T::get_network_master() == T::get_tree()->get_network_unique_id());
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
    return (NetworkManager::is_host() && is_server()) || !is_server();
}

