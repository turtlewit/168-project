tool
extends EditorPlugin


func _enter_tree():
	var icon : Texture = load("res://Textures/Tools/NetworkIcon.png")
	add_custom_type("NetworkIdentity", "Node", load("res://Scripts/Net/NetworkIdentity.gdns"), icon)
	add_custom_type("NetworkTransform", "Node", load("res://Scripts/Net/NetworkTransform.gdns"), icon)
	add_custom_type("NetworkSync", "Node", load("res://Scripts/Net/NetworkSync.gdns"), icon)


func _exit_tree():
	remove_custom_type("NetworkSync");
	remove_custom_type("NetworkTransform");
	remove_custom_type("NetworkIdentity");
