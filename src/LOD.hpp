#pragma once

#include <Godot.hpp>
#include <Spatial.hpp>
#include <Camera.hpp>

class LOD : public godot::Spatial {
	GODOT_CLASS(LOD, godot::Spatial)

public:
	static void _register_methods();

	void _init();
	void _ready();
	void _process(float delta);

private:
	void find_camera();
	int get_lod_number();

	godot::Camera* camera;

	float distance;
	godot::Array lod_paths;

	int current_lod;
};
