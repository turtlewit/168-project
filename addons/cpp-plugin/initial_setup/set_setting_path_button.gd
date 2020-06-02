tool
extends Button

export var project_setting_name: String

onready var path_label: RichTextLabel = get_node("../HBoxContainer/Label2")
onready var file_dialog: FileDialog = $FileDialog

func _on_FileDialog_dir_selected(dir: String) -> void:
	if not dir.ends_with('/'):
		dir += '/'
	path_label.text = dir
	ProjectSettings.set_setting(project_setting_name, dir)


func _on_FileDialog_gdnlib_file_selected(path: String) -> void:
	path_label.text = path
	ProjectSettings.set_setting(project_setting_name, path)
