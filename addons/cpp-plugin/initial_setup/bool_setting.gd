tool
extends CheckBox


export var project_setting_name: String

func _on_CheckBox_toggled(button_pressed: bool) -> void:
	ProjectSettings.set_setting(project_setting_name, button_pressed)
