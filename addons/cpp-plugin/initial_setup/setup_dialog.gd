tool
extends WindowDialog

func _on_Done_pressed() -> void:
	visible = false


func _on_SetupCDialog_popup_hide() -> void:
	var library_path: String = ProjectSettings.get_setting("c++/files/library_path")
	if len(library_path) > 0:
		var library_file := File.new()
		if not library_file.file_exists(library_path):
			var library := GDNativeLibrary.new()
			library.name = library_path
			ResourceSaver.save(ProjectSettings.get_setting("c++/files/library_path"), library, ResourceSaver.FLAG_CHANGE_PATH)
	ProjectSettings.set_setting("c++/plugin/plugin_set_up", true)
	ProjectSettings.save()
