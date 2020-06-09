tool
extends EditorPlugin

const MainPanel = preload("res://addons/cpp-plugin/main_panel/main_panel.tscn")
const NewClassDialog = preload("res://addons/cpp-plugin/new_class_dialog/new_class_dialog.tscn")
const SetupDialog = preload("res://addons/cpp-plugin/initial_setup/setup_c++_dialog.tscn")
const AddExistingDialog = preload("res://addons/cpp-plugin/add_existing_class/add_existing_class.tscn")

const Util := preload("res://addons/cpp-plugin/util.gd")

const SupportedBuildPlatforms := ['X11', 'Windows', 'OSX']

var main_panel_instance: Control
var new_class_dialog_instance: Popup
var custom_build_button: Button
var setup_dialog_instance: Popup
var add_existing_dialog_instance: Popup

var editor_settings: EditorSettings

func initialize_project_settings_property(name: String, default_value, type: int, property_hint: int = 0, hint_string: String = "") -> void:
	if not ProjectSettings.has_setting(name):
		ProjectSettings.set_setting(name, default_value)
		ProjectSettings.set_initial_value(name, default_value)
	ProjectSettings.add_property_info({
		"name": name,
		"type": type,
		"hint": property_hint,
		"hint_string": hint_string
	})

func initialize_editor_settings_property(name: String, default_value, type: int, property_hint: int = 0, hint_string: String = "") -> void:
	if not editor_settings.has_setting(name):
		editor_settings.set_setting(name, default_value)
		editor_settings.set_initial_value(name, default_value, false)
	editor_settings.add_property_info({
		"name": name,
		"type": type,
		"hint": property_hint,
		"hint_string": hint_string
	})

func _enter_tree() -> void:
	editor_settings = get_editor_interface().get_editor_settings()
	
	for platform in SupportedBuildPlatforms:
		#initialize_project_settings_property("c++/build_settings/build_on_files_changed", false, TYPE_BOOL)
		initialize_project_settings_property("c++/build_settings/%s/build_command" % platform, "", TYPE_STRING, PROPERTY_HINT_MULTILINE_TEXT)
		#initialize_project_settings_property("c++/build_settings/build_command_working_directory", "res://", TYPE_STRING, PROPERTY_HINT_DIR)
	initialize_project_settings_property("c++/files/source_path", "res://src/", TYPE_STRING, PROPERTY_HINT_DIR)
	initialize_project_settings_property("c++/files/header_subdirectory", "", TYPE_STRING)
	initialize_project_settings_property("c++/files/cpp_subdirectory", "", TYPE_STRING)
	initialize_project_settings_property("c++/files/library_path", "res://library.gdnlib", TYPE_STRING, PROPERTY_HINT_FILE)
	initialize_project_settings_property("c++/files/gdlibrary_source_file_path", "res://src/gdlibrary.cpp", TYPE_STRING, PROPERTY_HINT_FILE)
	initialize_project_settings_property("c++/files/scripts_directory", "res://Scripts/", TYPE_STRING, PROPERTY_HINT_DIR)
	initialize_project_settings_property("c++/files/c++_data_directory", "res://addons/cpp-plugin/data/", TYPE_STRING, PROPERTY_HINT_DIR)
	initialize_project_settings_property("c++/style/use_pragma_once", true, TYPE_BOOL)
	initialize_project_settings_property("c++/style/brace_style", 0, TYPE_INT, PROPERTY_HINT_ENUM, "K&R,1TBS,Allman")
	initialize_project_settings_property("c++/style/header_extension", 0, TYPE_INT, PROPERTY_HINT_ENUM, ".hpp,.h,.hxx,.hh")
	initialize_project_settings_property("c++/style/source_extension", 0, TYPE_INT, PROPERTY_HINT_ENUM, ".cpp,.cxx,.cc")
	initialize_project_settings_property("c++/style/indentation", 0, TYPE_INT, PROPERTY_HINT_ENUM, "Spaces,Tabs")

	if not ProjectSettings.has_setting("c++/plugin/plugin_set_up") or ProjectSettings.get_setting("c++/plugin/plugin_set_up") == false:
		initialize_project_settings_property("c++/plugin/plugin_set_up", false, TYPE_BOOL)
		setup_dialog_instance = SetupDialog.instance()
		get_editor_interface().get_editor_viewport().add_child(setup_dialog_instance)
		setup_dialog_instance.popup_centered()
		yield(setup_dialog_instance, "done")

	main_panel_instance = MainPanel.instance()
	add_control_to_bottom_panel(main_panel_instance, "C++")
	make_visible(false)

	new_class_dialog_instance = NewClassDialog.instance()
	get_editor_interface().get_editor_viewport().add_child(new_class_dialog_instance)
	new_class_dialog_instance.connect("update_class_tree", main_panel_instance.get_node("HBoxContainer/Tree"), "refresh")

	main_panel_instance.get_node("MainPanel/NewClass").connect("pressed", self, "_on_new_class_button_pressed")
	
	custom_build_button = Button.new()
	custom_build_button.text = "C++ Build"
	custom_build_button.connect("pressed", self, "build_lib")
	add_control_to_container(CONTAINER_TOOLBAR, custom_build_button)
	
	add_existing_dialog_instance = AddExistingDialog.instance()
	get_editor_interface().get_editor_viewport().add_child(add_existing_dialog_instance)
	main_panel_instance.get_node("MainPanel/AddExisting").connect("pressed", add_existing_dialog_instance, "popup_centered")
	add_existing_dialog_instance.connect("update_class_tree", main_panel_instance.get_node("HBoxContainer/Tree"), "refresh")

	var regen_button: Button = main_panel_instance.get_node("MainPanel/ReGenerate")
	regen_button.text = "Re-Generate " + ProjectSettings.get_setting("c++/files/gdlibrary_source_file_path").get_file()
	regen_button.connect("pressed", Util, "update_gdlibrary")
	
	add_tool_menu_item("Regenerate C++ Template Files", self, "_on_regenerate_templates_button_pressed")
	

func _exit_tree() -> void:
	remove_tool_menu_item("Regenerate C++ Template Files")
	if add_existing_dialog_instance:
		add_existing_dialog_instance.queue_free()
	if setup_dialog_instance:
		setup_dialog_instance.queue_free()
	if custom_build_button:
		remove_control_from_container(CONTAINER_TOOLBAR, custom_build_button)
		custom_build_button.queue_free()
	if new_class_dialog_instance:
		new_class_dialog_instance.queue_free()
	if main_panel_instance:
		remove_control_from_bottom_panel(main_panel_instance)
		main_panel_instance.queue_free()

func has_main_screen() -> bool:
	return false
	
func make_visible(visible: bool):
	if main_panel_instance:
		main_panel_instance.visible = visible
	
func get_plugin_name() -> String:
	return "CPP Plugin"
	
func get_plugin_icon() -> Texture:
	return get_editor_interface().get_base_control().get_icon("Node", "EditorIcons")

func _on_new_class_button_pressed() -> void:
	new_class_dialog_instance.popup_centered()

func build_lib() -> void:
	var executable := ""
	var args: PoolStringArray
	match OS.get_name():
		"X11":
			executable = "/bin/sh"
			args.append("-c")
		"OSX":
			executable = "/bin/sh"
			args.append("-c")
		"Windows":
			executable = "cmd.exe"
			args.append("/c")
		_:
			Util.oneshot_error_popup(get_editor_interface().get_editor_viewport(), "Cannot build on unsupported platform " + OS.get_name() + ".\nPlease open an issue on GitLab if you would like this platform to be supported.")
	
	if len(ProjectSettings.get_setting("c++/build_settings/%s/build_command" % OS.get_name())) == 0:
		Util.oneshot_error_popup(get_editor_interface().get_editor_viewport(), "Build command has not been set. Please set the build command in \"Projects->Project Settings...->C++->Build Settings\"")
		return

	var output := []
	args.append(ProjectSettings.get_setting("c++/build_settings/%s/build_command" % OS.get_name()))
	OS.execute(executable, args, true, output, true)
	for line in output:
		main_panel_instance.get_node("HBoxContainer/BuildLog").text += line

func _on_regenerate_templates_button_pressed(userdata):
	Util.write_templates()
