#include <LineEdit.hpp>
#include <SceneTree.hpp>

#include "Menu/HostDialogue.hpp"
#include "Net/NetworkManager.hpp"

using namespace godot;

void HostDialogue::_register_methods()
{
    register_signal<HostDialogue>("host", Dictionary::make(
        String("port"), Variant::Type::INT));
    
    register_method("_on_host_button_pressed", &HostDialogue::_on_host_button_pressed);
    register_method("_input", &HostDialogue::_input);
}

void HostDialogue::_enter_tree()
{
}

void HostDialogue::_exit_tree()
{
}

void HostDialogue::_init()
{
}

void HostDialogue::_input(InputEvent* e) 
{
	if (e->is_action_pressed("ui_cancel")) {
		get_parent()->set("visible", false);
	}
}

void HostDialogue::_on_host_button_pressed()
{
    LineEdit* port_line = cast_to<LineEdit>(get_node(NodePath(port_line_path)));

    int64_t port = port_line->get_text().to_int();

    emit_signal(String("host"), Array::make(port));

    //get_tree()->change_scene(String("res://Scenes/NetworkTest.tscn"));

    cast_to<CanvasItem>(get_node("../../../GameUI"))->set_visible(true);

    NetworkManager::get_singleton()->set_port(port);
    NetworkManager::get_singleton()->call_deferred("start_host");

    get_node("../../../CameraRotating")->queue_free();

    get_parent()->get_parent()->queue_free();
}
