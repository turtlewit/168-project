tool
extends LineEdit

export var project_setting_name: String

func _on_LineEdit_text_entered(new_text: String) -> void:
	ProjectSettings.set_setting(project_setting_name, new_text)
