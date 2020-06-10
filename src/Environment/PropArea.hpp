#pragma once

#include <Godot.hpp>
#include <Spatial.hpp>
#include <Shape.hpp>
#include <PackedScene.hpp>
#include <MultiMeshInstance.hpp>
#include <Transform.hpp>
#include <Path.hpp>
#include <Curve2D.hpp>

class PropArea : public godot::MultiMeshInstance {
	GODOT_CLASS(PropArea, godot::MultiMeshInstance)

public:
	static void _register_methods();

	void _init();
	void _notification(int what);
	void _ready();
	void _process(float delta);
	godot::String _get_configuration_warning();

	void _on_path_curve_changed();

	int64_t get_number_of_instances() { return number_of_instances; }
	void set_number_of_instances(int64_t value)
	{
		number_of_instances = value;
		if (get_multimesh()->get_instance_count() != number_of_instances)
			refresh();
	}

private:
	void refresh();
	void find_path();
	void update_curve();
	godot::Spatial* get_prefab_root();
	void clear_prefabs();

	godot::Ref<godot::PackedScene> prefab;
	godot::Vector3 base_position;
	godot::Vector3 base_rotation;
	godot::Vector3 base_scale;
	int64_t number_of_instances;
	bool randomize_rotation;
	float min_distance;
	int64_t collision_mask;

	godot::Path* path;
	godot::Ref<godot::Curve2D> curve2d;
};
