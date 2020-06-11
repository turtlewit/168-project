#pragma once

#include <Godot.hpp>
#include <Control.hpp>
#include <InputEvent.hpp>


// Connection Dialogue
//
// Signals:
//   - connect(ip, port)
//     - Emitted when the "Connect" button is pressed.
class ConnectionDialogue : public godot::Control {
    GODOT_CLASS(ConnectionDialogue, godot::Control)

public:
    static void _register_methods();

    void _init();

    void _enter_tree();
    void _exit_tree();

    void _input(godot::InputEvent* e);

    void _on_connect_button_pressed();

private:
    const char* ip_line_path = "VBoxContainer/IP Input/LineEdit";
    const char* port_line_path = "VBoxContainer/Port Input/LineEdit";
};
