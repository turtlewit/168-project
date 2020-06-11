#include <LineEdit.hpp>
#include <SceneTree.hpp>

#include "Menu/ConnectionDialogue.hpp"
#include "Net/NetworkManager.hpp"

using namespace godot;

void ConnectionDialogue::_register_methods()
{
    register_signal<ConnectionDialogue>("connect", Dictionary::make(
        String("ip"), Variant::Type::STRING, 
        String("port"), Variant::Type::INT,
	String("name"), Variant::Type::STRING));
    
    register_method("_on_connect_button_pressed", &ConnectionDialogue::_on_connect_button_pressed);
    register_method("_input", &ConnectionDialogue::_input);
}

void ConnectionDialogue::_enter_tree()
{
}

void ConnectionDialogue::_exit_tree()
{
}

void ConnectionDialogue::_init()
{
}

void ConnectionDialogue::_input(InputEvent* e)
{
	if (e->is_action_pressed("ui_cancel")) {
		get_parent()->set("visible", false);
	}
}

void ConnectionDialogue::_on_connect_button_pressed()
{
    LineEdit* ip_line = cast_to<LineEdit>(get_node(NodePath(ip_line_path)));
    LineEdit* port_line = cast_to<LineEdit>(get_node(NodePath(port_line_path)));
    LineEdit* name_line = cast_to<LineEdit>(get_node(NodePath("VBoxContainer/Username/LineEdit")));

    String ip = ip_line->get_text();
    int64_t port = port_line->get_text().to_int();
    String username = name_line->get_text();

    emit_signal(String("connect"), Array::make(ip, port, username));

    get_node("/root/LocalPlayerName")->set("username", username);

    //get_tree()->change_scene(String("res://Scenes/NetworkTest.tscn"));

    cast_to<CanvasItem>(get_node("../../../GameUI"))->set_visible(true);

    NetworkManager::get_singleton()->set_address(ip);
    NetworkManager::get_singleton()->set_port(port);
    NetworkManager::get_singleton()->call_deferred("start_client");

    get_node("../../../CameraRotating")->queue_free();

    get_parent()->get_parent()->queue_free();
}
