const Util := preload("res://addons/cpp-plugin/util.gd")

static func create_class(name: String, base_class: String, subpath: String, is_tool: bool) -> void:
	#var header_template_path: String# = "res://addons/cpp-plugin/template.hpp"	
	#var source_template_path: String# = "res://addons/cpp-plugin/template.cpp"
	var header_template_path: String = Util.add_slash(ProjectSettings.get_setting("c++/files/c++_data_directory")) + "template.hpp"
	var source_template_path: String = Util.add_slash(ProjectSettings.get_setting("c++/files/c++_data_directory")) + "template.cpp"
	
	#match ProjectSettings.get_setting("c++/style/indentation_style"):
	#	0: # K&R
	#		header_template_path = "res://addons/cpp-plugin/template_kr1tbs.hpp"
	#		source_template_path = "res://addons/cpp-plugin/template_krallman.cpp"
	#	1: # 1TBS
	#		header_template_path = "res://addons/cpp-plugin/template_kr1tbs.hpp"
	#		source_template_path = "res://addons/cpp-plugin/template_1tbs.cpp"
	#	2: # Allman
	#		header_template_path = "res://addons/cpp-plugin/template_allman.hpp"
	#		source_template_path = "res://addons/cpp-plugin/template_krallman.cpp"

	var header_dir_path: String = Util.add_slash(ProjectSettings.get_setting("c++/files/source_path")) + Util.add_slash(ProjectSettings.get_setting("c++/files/header_subdirectory")) + Util.add_slash(subpath)
	var source_dir_path: String = Util.add_slash(ProjectSettings.get_setting("c++/files/source_path")) + Util.add_slash(ProjectSettings.get_setting("c++/files/cpp_subdirectory")) + Util.add_slash(subpath)
	
	var ext_h := Util.get_header_extension()
	var ext_s := Util.get_source_extension()

	var header_path: String = header_dir_path + name + ext_h
	var source_path: String = source_dir_path + name + ext_s
	
	Util.make_dir(header_dir_path)
	Util.make_dir(source_dir_path)
	
	# Create the source and header files based on their respective templates.
	var source_template := Util.read_file(source_template_path)
	var header_template := Util.read_file(header_template_path)

	
	var new_source := source_template.format({
		"header-file": Util.add_slash(ProjectSettings.get_setting("c++/files/header_subdirectory")) + name + ext_h,
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
	Util.overwrite_file(source_path, new_source)
	Util.overwrite_file(header_path, new_header)
	
	var script_dir_path: String = Util.add_slash(ProjectSettings.get_setting("c++/files/scripts_directory")) + Util.add_slash(subpath)
	
	Util.make_dir(script_dir_path)
	
	# Create, fill out, and save the new script resource.
	var script_resource := NativeScript.new()
	script_resource.set("class_name", name)
	script_resource.library = load(ProjectSettings.get_setting("c++/files/library_path"))
	script_resource.script_class_name = name
	
	ResourceSaver.save(script_dir_path + name + ".gdns", script_resource, ResourceSaver.FLAG_CHANGE_PATH)
	
	var class_info := Util.get_class_info()
	# Append the class header path and class name to the class_header_paths dictionary.
	class_info.class_info.append({
		"name": name,
		"subdir": Util.add_slash(subpath),
		"header": Util.add_slash(ProjectSettings.get_setting("c++/files/header_subdirectory")) + Util.add_slash(subpath) + name + ext_h,
		"is_tool": is_tool})
	ResourceSaver.save(class_info.resource_path, class_info)
	
	Util.update_gdlibrary()
