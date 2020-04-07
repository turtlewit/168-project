#include <OS.hpp>

#include "Net/NetworkStarter.hpp"
#include "Net/NetworkManager.hpp"

using namespace godot;

void NetworkStarter::_register_methods()
{
	register_property(
		"start_mode", 
		&NetworkStarter::set_start_mode,
		&NetworkStarter::get_start_mode,
		static_cast<int64_t>(NetworkStartMode::HOST),
		GODOT_METHOD_RPC_MODE_DISABLED, 
		GODOT_PROPERTY_USAGE_DEFAULT, 
		GODOT_PROPERTY_HINT_ENUM, 
		String("HOST,SERVER,CLIENT"));
	register_property("autostart", &NetworkStarter::autostart, true);

	register_method("_start_network", &NetworkStarter::start_network);
	register_method("_ready", &NetworkStarter::_ready);
}

void NetworkStarter::start_network()
{
	NetworkManager* network_manager = NetworkManager::get_singleton();

	PoolStringArray args = OS::get_singleton()->get_cmdline_args();
	for (int i = 0; i < args.size(); ++i) {
		const String arg = args[i];
		if (arg == "--host") {
			start_mode = NetworkStartMode::HOST;
			break;
		}

		if (arg == "--server") {
			start_mode = NetworkStartMode::SERVER;
			break;
		}
		
		if (arg == "--client") {
			start_mode = NetworkStartMode::CLIENT;
			break;
		}
	}

	switch (start_mode) {
	case NetworkStartMode::HOST:
		network_manager->start_host();
		break;
	case NetworkStartMode::SERVER:
		network_manager->start_server();
		break;
	case NetworkStartMode::CLIENT:
		network_manager->start_client();
		break;
	}

	Godot::print("Starting network...");

}

void NetworkStarter::_init()
{
}

void NetworkStarter::_ready()
{
	if (!autostart)
		return;

	start_network();
}

NetworkStarter::NetworkStarter()
	: autostart{true}, start_mode{NetworkStartMode::HOST}
{
}