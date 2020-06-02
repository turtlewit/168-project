tool
extends WindowDialog

signal update_class_tree

const class_header_paths: Resource = preload("res://addons/cpp-plugin/class_header_paths/class_header_paths.tres")
const create_class := preload("res://addons/cpp-plugin/create_class.gd")
const util := preload("res://addons/cpp-plugin/util.gd")

onready var name_line: LineEdit = $VBoxContainer2/VBoxContainer/ClassName/LineEdit
onready var subdir_line: LineEdit = $VBoxContainer2/VBoxContainer/Subdir/LineEdit
onready var header_line: LineEdit = $VBoxContainer2/VBoxContainer/Header/HBoxContainer/LineEdit

func _on_Button_pressed() -> void:
	var srcdir: String = ProjectSettings.get_setting("c++/files/source_path")
	if not header_line.text.begins_with(srcdir):
		printerr("Header file is not in " + srcdir + ". Cannot add existing class.")
		return
	
	var header: String = header_line.text.right(len(srcdir))
	if header[0] == '/':
		header = header.right(1)
	class_header_paths.class_header_paths.append({
		"name": name_line.text,
		"subdir": util.add_slash(subdir_line.text),
		"header": header})
	ResourceSaver.save(class_header_paths.resource_path, class_header_paths, ResourceSaver.FLAG_BUNDLE_RESOURCES)
	util.update_gdlibrary()
	emit_signal("update_class_tree")
	visible = false
