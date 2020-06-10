#include <SceneTree.hpp>
#include <Viewport.hpp>

#include "LOD.hpp"
#include "Utils/Defs.hpp"

using namespace godot;

void LOD::_register_methods()
{
	register_method("_ready", &LOD::_ready);
	register_method("_process", &LOD::_process);

	register_property("distance", &LOD::distance, 200.0f);
	register_property("lod_paths", &LOD::lod_paths, Array());
	REGISTER_PROPERTY_HINT(LOD, Array, lod_paths, Array(), GODOT_PROPERTY_HINT_TYPE_STRING, GODOT_HINT_STRING_ARRAY_OF_NODE_PATHS);
}

void LOD::_init()
{
	distance = 200.0f;
}

void LOD::_ready()
{
	find_camera();
}

void LOD::_process(float delta)
{
	if (!camera) {
		find_camera();
		if (!camera)
			return;
	}

	if (camera->is_current() != true) {
		find_camera();
		if (!camera || camera->is_current() != true)
			return;
	}

	int new_lod = get_lod_number();

	if (current_lod != new_lod) {
		cast_to<Spatial>(get_node(lod_paths[current_lod]))->set_visible(false);
		cast_to<Spatial>(get_node(lod_paths[new_lod]))->set_visible(true);
		current_lod = new_lod;
	}
}

void LOD::find_camera()
{
	camera = get_tree()->get_root()->get_camera();
}

int LOD::get_lod_number()
{
	float dist = get_global_transform().origin.distance_to(camera->get_global_transform().origin);
	int nlod = lod_paths.size() - 1;

	if (dist >= distance)
		return nlod;

	return static_cast<int>((dist*nlod)/distance);
}

