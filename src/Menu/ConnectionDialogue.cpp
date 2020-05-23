#include <LineEdit.hpp>
#include <SceneTree.hpp>

#include "Menu/ConnectionDialogue.hpp"
#include "Net/NetworkManager.hpp"

using namespace godot;

void ConnectionDialogue::_register_methods()
{
    register_signal<ConnectionDialogue>("connect", Dictionary::make(
        String("ip"), Variant::Type::STRING, 
        String("port"), Variant::Type::INT));
    
    register_method("_on_connect_button_pressed", &ConnectionDialogue::_on_connect_button_pressed);
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

void ConnectionDialogue::_on_connect_button_pressed()
{
    LineEdit* ip_line = cast_to<LineEdit>(get_node(NodePath(ip_line_path)));
    LineEdit* port_line = cast_to<LineEdit>(get_node(NodePath(port_line_path)));

    String ip = ip_line->get_text();
    int64_t port = port_line->get_text().to_int();

    emit_signal(String("connect"), Array::make(ip, port));

    //get_tree()->change_scene(String("res://Scenes/NetworkTest.tscn"));

    cast_to<CanvasItem>(get_node("../../../GameUI"))->set_visible(true);

    NetworkManager::get_singleton()->set_address(ip);
    NetworkManager::get_singleton()->set_port(port);
    NetworkManager::get_singleton()->call_deferred("start_client");

    get_node("../../../CameraRotating")->queue_free();

    get_parent()->get_parent()->queue_free();
}