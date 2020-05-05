#include <Godot.hpp>

#include "Hello.hpp"
#include "Player/Player.hpp"
#include "Net/NetworkManager.hpp"
#include "Net/NetworkStarter.hpp"
#include "Net/NetworkSync.hpp"
#include "Net/NetworkTransform.hpp"
#include "Objects/Crystal.hpp"
#include "Net/NetworkIdentity.hpp"
#include "Net/NetworkAnimator.hpp"
#include "Objects/Portal.hpp"

extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options* o) 
{
    godot::Godot::gdnative_init(o);
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options* o) 
{
    godot::Godot::gdnative_terminate(o);
}

extern "C" void GDN_EXPORT godot_nativescript_init(void* handle) 
{
    godot::Godot::nativescript_init(handle);

    godot::register_class<Hello>();
    godot::register_class<godot::Player>();
    godot::register_class<NetworkManager>();
    godot::register_class<NetworkStarter>();
    godot::register_class<NetworkSync>();
    godot::register_class<NetworkTransform>();
    godot::register_class<godot::Crystal>();
    godot::register_class<NetworkIdentity>();
    godot::register_class<NetworkAnimator>();
    godot::register_class<godot::Portal>();
}
