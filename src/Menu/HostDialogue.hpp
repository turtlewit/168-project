#pragma once

#include <Godot.hpp>
#include <Control.hpp>

// Host Dialogue
//
// Signals:
//   - host(port)
//     - Emitted when the "Host" button is pressed.
class HostDialogue : public godot::Control {
    GODOT_CLASS(HostDialogue, godot::Control)

public:
    static void _register_methods();

    void _init();

    void _enter_tree();
    void _exit_tree();

    void _on_host_button_pressed();

private:
    const char* port_line_path = "VBoxContainer/Port Input/LineEdit";
};