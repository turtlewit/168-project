const source_template_path: String = "res://addons/cpp-plugin/template.cpp"
const header_template_path: String = "res://addons/cpp-plugin/template.hpp"

const class_header_paths: Resource = preload("res://addons/cpp-plugin/class_header_paths/class_header_paths.tres")

const util := preload("res://addons/cpp-plugin/util.gd")

static func create_class(name: String, base_class: String, subpath: String) -> void:
	var header_dir_path: String = util.add_slash(ProjectSettings.get_setting("c++/files/source_path")) + util.add_slash(ProjectSettings.get_setting("c++/files/header_subdirectory")) + util.add_slash(subpath)
	var source_dir_path: String = util.add_slash(ProjectSettings.get_setting("c++/files/source_path")) + util.add_slash(ProjectSettings.get_setting("c++/files/cpp_subdirectory")) + util.add_slash(subpath)
	var header_path: String = header_dir_path + name + ".hpp"
	var source_path: String = source_dir_path + name + ".cpp"
	
	util.make_dir(header_dir_path)
	util.make_dir(source_dir_path)
	
	# Create the source and header files based on their respective templates.
	var source_template_file := File.new()
	var header_template_file := File.new()
	source_template_file.open(source_template_path, File.READ)
	header_template_file.open(header_template_path, File.READ)
	
	var source_template := source_template_file.get_as_text()
	var header_template := header_template_file.get_as_text()
	
	source_template_file.close()
	header_template_file.close()
	
	var new_source := source_template.format({
		"header-file": ProjectSettings.get_setting("c++/files/header_subdirectory") + name + ".hpp",
		"class-name": name,
	})
	
	var top_include_guard := ""
	var bottom_include_guard := ""
	
	if ProjectSettings.get_setting("c++/style/use_pragma_once") == true:
		top_include_guard = "#pragma once"
	else:
		top_include_guard = """#ifndef {capital-class-name}_HPP
#define {capital-class-name}_HPP""".format({"capital-class-name": name.to_upper()})
		bottom_include_guard = """
#endif"""
	
	var new_header := header_template.format({
		"top-include-guard": top_include_guard,
		"base-class": base_class,
		"class-name": name,
		"bottom-include-guard": bottom_include_guard,
	})
	
	# Write the new source files.
	var new_source_file := File.new()
	var new_header_file := File.new()
	
	new_source_file.open(source_path, File.WRITE)
	new_header_file.open(header_path, File.WRITE)
	
	new_source_file.store_string(new_source)
	new_header_file.store_string(new_header)
	
	new_source_file.close()
	new_header_file.close()
	
	var script_dir_path: String = util.add_slash(ProjectSettings.get_setting("c++/files/scripts_directory")) + util.add_slash(subpath)
	
	util.make_dir(script_dir_path)
	
	# Create, fill out, and save the new script resource.
	var script_resource := NativeScript.new()
	script_resource.set("class_name", name)
	script_resource.library = load(ProjectSettings.get_setting("c++/files/library_path"))
	script_resource.script_class_name = name
	
	ResourceSaver.save(script_dir_path + name + ".tres", script_resource, ResourceSaver.FLAG_CHANGE_PATH)
	
	# Append the class header path and class name to the class_header_paths dictionary.
	class_header_paths.class_header_paths.append({
		"name": name,
		"subdir": util.add_slash(subpath),
		"header": util.add_slash(ProjectSettings.get_setting("c++/files/header_subdirectory")) + util.add_slash(subpath) + name + ".hpp"})
	ResourceSaver.save(class_header_paths.resource_path, class_header_paths, ResourceSaver.FLAG_BUNDLE_RESOURCES)
	
	util.update_gdlibrary()
