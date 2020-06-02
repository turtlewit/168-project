extends Object

const class_header_paths: Resource = preload("res://addons/cpp-plugin/class_header_paths/class_header_paths.tres")

static func get_class_headers() -> String:
	var header_string := ""
	
	for info in class_header_paths.class_header_paths:
		header_string += "#include \"{path}\"\n".format({"path": info["header"]})
	
	return header_string

static func get_register_classes() -> String:
	var class_names := ""
	
	for info in class_header_paths.class_header_paths:
		class_names += "    register_class<{class-name}>();\n".format({"class-name": info["name"]})
	
	return class_names

static func update_gdlibrary() -> void:
	# Write an updated gdlibrary source file based on the gdlibrary template.
	var gdlibrary_template_file := File.new()
	gdlibrary_template_file.open("res://addons/cpp-plugin/gdlibrary_template.cpp", File.READ)
	
	var gdlibrary_template := gdlibrary_template_file.get_as_text()
	
	gdlibrary_template_file.close()
	
	var gdlibrary_source_file := File.new()
	gdlibrary_source_file.open(ProjectSettings.get_setting("c++/files/gdlibrary_source_file_path"), File.WRITE)
	
	gdlibrary_source_file.store_string(gdlibrary_template.format({
		"class-headers": get_class_headers(),
		"register-classes": get_register_classes(),
	}))
	
	gdlibrary_source_file.close()

static func oneshot_error_popup(parent: Control, text: String) -> void:
	var error := AcceptDialog.new()
	error.connect("popup_hide", error, "queue_free")
	error.dialog_text = text
	parent.add_child(error)
	error.popup_centered()

static func add_slash(t: String) -> String:
	if (len(t) == 0):
		return t
	var r := t
	if t[len(t)-1] != '/':
		r += '/'
	return r

static func make_dir(path: String) -> void:
	var dir := Directory.new()
	if not dir.dir_exists(path):
		dir.make_dir_recursive(path)
