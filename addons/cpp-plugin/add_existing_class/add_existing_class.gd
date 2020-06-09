tool
extends WindowDialog

signal update_class_tree

const CreateClass := preload("res://addons/cpp-plugin/create_class.gd")
const Util := preload("res://addons/cpp-plugin/util.gd")

onready var name_line: LineEdit = $VBoxContainer2/VBoxContainer/ClassName/LineEdit
onready var subdir_line: LineEdit = $VBoxContainer2/VBoxContainer/Subdir/LineEdit
onready var header_line: LineEdit = $VBoxContainer2/VBoxContainer/Header/HBoxContainer/LineEdit
onready var is_tool: CheckBox = $VBoxContainer2/VBoxContainer/IsTool/CheckBox

func reset() -> void:
	name_line.text = ""
	subdir_line.text = ""
	is_tool.pressed = false

func _on_Button_pressed() -> void:
	var srcdir: String = ProjectSettings.get_setting("c++/files/source_path")
	if not header_line.text.begins_with(srcdir):
		printerr("Header file is not in " + srcdir + ". Cannot add existing class.")
		return
		
	var class_info := Util.get_class_info()
	var header: String = header_line.text.right(len(srcdir))
	if header[0] == '/':
		header = header.right(1)
	class_info.class_info.append({
		"name": name_line.text,
		"subdir": Util.add_slash(subdir_line.text),
		"header": header,
		"is_tool": is_tool.pressed})
	ResourceSaver.save(class_info.resource_path, class_info)
	Util.update_gdlibrary()
	emit_signal("update_class_tree")
	visible = false


func _on_AddExistingClass_visibility_changed() -> void:
	if visible == false:
		reset()
