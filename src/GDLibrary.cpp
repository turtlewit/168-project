#include <Godot.hpp>

using namespace godot;

// Project includes
#include "Menu/ConnectionDialogue.hpp"
#include "Menu/HostDialogue.hpp"
#include "Net/NetworkAnimator.hpp"
#include "Net/NetworkIdentity.hpp"
#include "Net/NetworkManager.hpp"
#include "Net/NetworkStarter.hpp"
#include "Net/NetworkSync.hpp"
#include "Net/NetworkTransform.hpp"
#include "Net/NetworkSignalManager.hpp"
#include "Objects/Crystal.hpp"
#include "Objects/Portal.hpp"
#include "Objects/DeathPlane.hpp"
#include "System/SignalManagerPlayer.hpp"
#include "System/GameUI.hpp"
#include "GrassMultimesh.hpp"
#include "Player/Player.hpp"
#include "Environment/PropArea.hpp"


// godot_gdnative_init
extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options* o)
{
	godot::Godot::gdnative_init(o);
}

// godot_gdnative_terminate
extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options* o)
{
	godot::Godot::gdnative_terminate(o);
}

// godot_nativescript_init
extern "C" void GDN_EXPORT godot_nativescript_init(void* handle)
{
	godot::Godot::nativescript_init(handle);

	register_class<ConnectionDialogue>();
	register_class<HostDialogue>();
	register_class<NetworkAnimator>();
	register_class<NetworkIdentity>();
	register_class<NetworkManager>();
	register_class<NetworkStarter>();
	register_class<NetworkSync>();
	register_class<NetworkTransform>();
	register_class<NetworkSignalManager>();
	register_class<Crystal>();
	register_class<Portal>();
	register_class<DeathPlane>();
	register_class<SignalManagerPlayer>();
	register_class<GameUI>();
	register_class<GrassMultimesh>();
	register_class<Player>();
	register_tool_class<PropArea>();

}
