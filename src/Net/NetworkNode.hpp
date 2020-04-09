#pragma once

#include <Godot.hpp>
#include <Node.hpp>
#include <SceneTree.hpp>

#include <Net/NetworkManager.hpp>

template <typename T, typename C>
class NetworkNode : public T {
public:
    static void _register_methods();

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
    if (T::get_tree()->has_network_peer()) {
        _on_network_start();
        return;
    }

    T::set_process(false);

    NetworkManager::get_singleton()->connect("network_connected", this, "__on_network_start", godot::Array(), T::CONNECT_REFERENCE_COUNTED);
}