tool
extends Popup

const CreateClass = preload("res://addons/cpp-plugin/create_class.gd")

onready var name_of_class: LineEdit = $"VBoxContainer2/VBoxContainer/Class Name/LineEdit"
onready var base_class: LineEdit = $"VBoxContainer2/VBoxContainer/Inherits From/LineEdit"
onready var subdir: LineEdit = $"VBoxContainer2/VBoxContainer/Subdirectory/LineEdit"
onready var is_tool: CheckBox = $"VBoxContainer2/VBoxContainer/IsTool/CheckBox"

signal update_class_tree

func reset() -> void:
	name_of_class.text = ""
	base_class.text = "Node"
	subdir.text = ""
	is_tool.pressed = false

func _on_CreateButton_button_down() -> void:
	CreateClass.create_class(name_of_class.text, base_class.text, subdir.text, is_tool.pressed)
	emit_signal("update_class_tree")
	visible = false


func _on_NewClassDialog_visibility_changed() -> void:
	if (visible == false):
		reset()
