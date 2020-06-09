extends Object

const ClassInfo := preload("res://addons/cpp-plugin/class_info/class_info.gd")

const GdlibraryTemplateTemplate: String = """#include <Godot.hpp>

using namespace godot;

// Project includes
{class-headers}

// godot_gdnative_init
extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options* o){brace-newline}{
{indent}godot::Godot::gdnative_init(o);
}

// godot_gdnative_terminate
extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options* o){brace-newline}{
{indent}godot::Godot::gdnative_terminate(o);
}

// godot_nativescript_init
extern "C" void GDN_EXPORT godot_nativescript_init(void* handle){brace-newline}{
{indent}godot::Godot::nativescript_init(handle);

{register-classes}
}
"""

const SourceTemplateTemplate: String = """#include "{header-file}"

using namespace godot;

void {class-name}::_register_methods(){brace-newline}{
{indent}register_method("_ready", &{class-name}::_ready);
{indent}register_method("_process", &{class-name}::_process);
}

void {class-name}::_init(){brace-newline}{
}

void {class-name}::_ready(){brace-newline}{
}

void {class-name}::_process(float delta){brace-newline}{
}

{class-name}::{class-name}(){brace-newline}{
}

{class-name}::~{class-name}(){brace-newline}{
}
"""
const HeaderTemplateTemplate: String = """{top-include-guard}

#include <Godot.hpp>
#include <{base-class}.hpp>

class {class-name} : public godot::{base-class}{brace-newline}{
{indent}GODOT_CLASS({class-name}, godot::{base-class})

public:
{indent}static void _register_methods();

{indent}void _init();
{indent}void _ready();
{indent}void _process(float delta);

{indent}{class-name}();
{indent}~{class-name}();
};{bottom-include-guard}
"""

static func get_class_info() -> ClassInfo:
	var path: String = ProjectSettings.get_setting("c++/files/c++_data_directory") + "/" + "class_info.tres"
	if not ResourceLoader.exists(path):
		printerr("Class info resource not found! Returning ClassInfo()")
		return ClassInfo.new()
	return load(path) as ClassInfo

static func write_templates() -> void:
	var gdlibrary_template_path: String = add_slash(ProjectSettings.get_setting("c++/files/c++_data_directory")) + "gdlibrary_template.cpp"
	var header_template_path: String = add_slash(ProjectSettings.get_setting("c++/files/c++_data_directory")) + "template.hpp"
	var source_template_path: String = add_slash(ProjectSettings.get_setting("c++/files/c++_data_directory")) + "template.cpp"
	var function_brace_newline := ""
	var class_brace_newline := ""
	match ProjectSettings.get_setting("c++/style/brace_style"):
		0: # K&R
			function_brace_newline = "\n"
			class_brace_newline = " "
		1: # 1TBS
			function_brace_newline = " "
			class_brace_newline = " "
		2: # Allman
			function_brace_newline = "\n"
			class_brace_newline = "\n"

	var indent = "    "

	if ProjectSettings.get_setting("c++/style/indentation") == 1: # replace spaces with tabs
		indent = "\t"

	var gdlibrary_template_text := GdlibraryTemplateTemplate.format({"brace-newline": function_brace_newline, "indent": indent})
	var source_template_text := SourceTemplateTemplate.format({"brace-newline": function_brace_newline, "indent": indent})
	var header_template_text := HeaderTemplateTemplate.format({"brace-newline": class_brace_newline, "indent": indent})

	overwrite_file(gdlibrary_template_path, gdlibrary_template_text)
	overwrite_file(source_template_path, source_template_text)
	overwrite_file(header_template_path, header_template_text)

static func get_class_headers() -> String:
	var header_string := ""
	
	for info in get_class_info().class_info:
		header_string += "#include \"{path}\"\n".format({"path": info["header"]})
	
	return header_string

static func get_register_classes() -> String:
	var class_names := ""
	var tab_character: String
	if ProjectSettings.get_setting("c++/style/indentation") == 0:
		tab_character = "    "
	else:
		tab_character = "\t"
	
	for info in get_class_info().class_info:
		if info.get("is_tool", false):
			class_names += "{tab-character}register_tool_class<{class-name}>();\n".format({"tab-character": tab_character, "class-name": info["name"]})
		else:
			class_names += "{tab-character}register_class<{class-name}>();\n".format({"tab-character": tab_character, "class-name": info["name"]})
	
	return class_names

static func update_gdlibrary() -> void:
	# Write an updated gdlibrary source file based on the gdlibrary template.
	var gdlibrary_template := read_file(add_slash(ProjectSettings.get_setting("c++/files/c++_data_directory")) + "gdlibrary_template.cpp")

	overwrite_file(ProjectSettings.get_setting("c++/files/gdlibrary_source_file_path"), gdlibrary_template.format({
		"class-headers": get_class_headers(),
		"register-classes": get_register_classes(),
	}))

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

static func get_header_extension() -> String:
	match ProjectSettings.get_setting("c++/style/header_extension"):
		0:
			return ".hpp"
		1:
			return ".h"
		2:
			return ".hxx"
		3:
			return ".hh"
	return ".hpp"

static func get_source_extension() -> String:
	match ProjectSettings.get_setting("c++/style/header_extension"):
		0:
			return ".cpp"
		1:
			return ".cxx"
		2:
			return ".cc"
	return ".cpp"

static func make_dir(path: String) -> void:
	var dir := Directory.new()
	if not dir.dir_exists(path):
		dir.make_dir_recursive(path)

static func read_file(path: String) -> String:
	var f := File.new()
	f.open(path, File.READ)
	var text := f.get_as_text()
	f.close()
	return text

static func overwrite_file(path: String, text: String) -> void:
	var f := File.new()
	f.open(path, File.WRITE)
	f.store_string(text)
	f.close()
