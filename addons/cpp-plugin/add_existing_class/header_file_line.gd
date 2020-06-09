tool
extends LineEdit

const Util := preload("res://addons/cpp-plugin/util.gd")

onready var header_subdir: String = ProjectSettings.get_setting("c++/files/source_path") + ProjectSettings.get_setting("c++/files/header_subdirectory")
var name_of_class := ""
var subdir := ""


func _ready() -> void:
	text = Util.add_slash(header_subdir) + ".hpp"

func update_text() -> void:
	text = Util.add_slash(header_subdir) + Util.add_slash(subdir) + name_of_class + ".hpp"

func _on_ClassName_LineEdit_text_changed(new_text: String) -> void:
	name_of_class = new_text
	update_text()

func _on_Subdir_LineEdit_text_changed(new_text: String) -> void:
	subdir = new_text
	update_text()
