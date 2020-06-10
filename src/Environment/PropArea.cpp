// PropArea.cpp
#define _USE_MATH_DEFINES
#include <vector>

#include <BoxShape.hpp>
#include <PhysicsDirectSpaceState.hpp>
#include <World.hpp>
#include <MultiMesh.hpp>
#include <Path2D.hpp>
#include <Curve2D.hpp>
#include <Rect2.hpp>
#include <PoolArrays.hpp>
#include <Geometry.hpp>
#include <Curve3D.hpp>
#include <Engine.hpp>
#include <Quat.hpp>
#include <SceneTree.hpp>

#include "PropArea.hpp"
#include "Utils/Defs.hpp"
#include "Utils/Mathf.hpp"

using namespace godot;

#ifndef MIN
# define MIN(a, b) a < b ? a : b
#endif

#ifndef MAX
# define MAX(a, b) a > b ? a : b
#endif

#define IN_EDITOR Engine::get_singleton()->is_editor_hint()

namespace {
	bool is_point_in_curve(const Vector2& point, const Ref<Curve2D> curve)
	{
		if (curve->get_point_count() == 0) {
			PRINT_ERROR("Curve has no points!", "is_point_in_curve");
			return true;
		}

		return Geometry::get_singleton()->is_point_in_polygon(point, curve->get_baked_points());
	}

	Rect2 get_curve_bounding_box(const Ref<Curve2D> curve)
	{
		if (curve->get_point_count() == 0) {
			PRINT_ERROR("Curve has no points!", "get_curve_bounding_box");
			return Rect2();
		}

		float min_x, max_x, min_y, max_y;
		auto points = curve->get_baked_points();
		min_x = max_x = points[0].x;
		min_y = max_y = points[0].y;
		for (int i = 1; i < points.size(); ++i)
		{
			min_x = MIN(points[i].x, min_x);
			min_y = MIN(points[i].y, min_y);
			max_x = MAX(points[i].x, max_x);
			max_y = MAX(points[i].y, max_y);
		}
		return Rect2(min_x, min_y, max_x - min_x, max_y - min_y);
	}
}

void PropArea::_register_methods()
{
	register_method("_notification", &PropArea::_notification);
	register_method("_ready", &PropArea::_ready);
	register_method("_process", &PropArea::_process);
	register_method("_get_configuration_warning", &PropArea::_get_configuration_warning);
	register_method("_on_path_curve_changed", &PropArea::_on_path_curve_changed);

	REGISTER_PROPERTY_HINT(PropArea, Ref<PackedScene>, prefab, Ref<PackedScene>(), GODOT_PROPERTY_HINT_RESOURCE_TYPE, "PackedScene");
	register_property<PropArea, int64_t>("number_of_instances", &PropArea::set_number_of_instances, &PropArea::get_number_of_instances, 0);
	register_property("randomize_rotation", &PropArea::randomize_rotation, false);
	register_property("base_position", &PropArea::base_position, Vector3());
	register_property("base_rotation", &PropArea::base_rotation, Vector3());
	register_property("base_scale", &PropArea::base_scale, Vector3(1, 1, 1));
	register_property("min_distance", &PropArea::min_distance, 0.0f);
	REGISTER_PROPERTY_HINT(PropArea, int64_t, collision_mask, -1, GODOT_PROPERTY_HINT_LAYERS_3D_PHYSICS, String());
}

void PropArea::_init()
{
	base_scale = Vector3(1, 1, 1);
	collision_mask = -1;
}

void PropArea::_notification(int what)
{
	switch (what) {
	case NOTIFICATION_TRANSFORM_CHANGED:
		refresh();
		break;
	}
}

void PropArea::_ready()
{
	curve2d = Ref(Curve2D::_new());
	curve2d->set_bake_interval(1.0f);
	find_path();
	update_configuration_warning();
	set_notify_transform(true);

	if (!has_node("Prefabs")) {
		Spatial* p = Spatial::_new();
		add_child(p);
		p->set_owner(get_tree()->get_edited_scene_root());
		p->set_name("Prefabs");
	}

	get_tree()->connect("tree_changed", this, "update_configuration_warning");
}

void PropArea::_process(float delta)
{
}

String PropArea::_get_configuration_warning()
{
	if (path) {
		if (path->get_curve()->get_point_count() == 0)
			return "Please add points to the path.";
		return String();
	}

	String path_warning = "Please add a Path node under this node.";

	find_path();
	if (!path)
		return path_warning;
	return String();
}

void PropArea::_on_path_curve_changed()
{
	if (path->get_curve()->get_point_count() > 0) {
		refresh();
	}
}

void PropArea::refresh()
{
	if (!path)
		return;
	if (!IN_EDITOR)
		return;
	if (path->get_curve()->get_point_count() == 0)
		return;
	if (has_meta("_edit_lock_"))
		return;

	update_curve();

	clear_prefabs();

	Spatial* prefabs = get_prefab_root();

	Ref<MultiMesh> multimesh = get_multimesh();
	if (multimesh == Ref<MultiMesh>())
		return;

	multimesh->set_instance_count(number_of_instances);

	auto space_state = get_world()->get_direct_space_state();
	Rect2 curve_box = get_curve_bounding_box(curve2d);

	bool use_min_dist = min_distance > 0;

	std::vector<Vector2> positions{};

	for (int64_t i = 0; i < number_of_instances; ++i) {
		Transform gt = get_global_transform();
		Transform t = Transform();
		t.scale(base_scale);
		t.rotate(Vector3(0, 1, 0), Mathf::deg2rad(base_rotation.y));
		t.rotate(Vector3(0, 0, 1), Mathf::deg2rad(base_rotation.z));
		t.rotate(Vector3(1, 0, 0), Mathf::deg2rad(base_rotation.x));
		t.translate(base_position);

		Vector2 offset;
		int j;
		for (j = 0; j < 1000; ++j) {
			offset = Vector2(Mathf::rand_range(curve_box.position.x, curve_box.position.x + curve_box.size.x), Mathf::rand_range(curve_box.position.y, curve_box.position.y + curve_box.size.y));

			if (use_min_dist) {
				bool good = true;
				for (const auto& p : positions) {
					if (offset.distance_to(p) < min_distance) {
						good = false;
						break;
					}
				}
				if (!good)
					continue;
			}

			if (is_point_in_curve(offset, curve2d))
				break;
		}

		if (j == 1000) {
			continue;
		}

		if (use_min_dist)
			positions.push_back(offset);

		Vector3 offset3 = Vector3(offset.x, 0, offset.y);

		auto result = space_state->intersect_ray(gt.origin + offset3, gt.origin + offset3 + Vector3(0, -1000.0f, 0), Array(), collision_mask);
		Vector3 newpos;
		if (!result.empty()) {
			newpos = result["position"];
		} else {
			newpos = offset3;
			newpos.y = gt.origin.y;
		}

		t.origin = Vector3(0, 0, 0);

		if (randomize_rotation) {
			t.rotate(Vector3(0, 1, 0), Mathf::rand_range(0, 2*M_PI));
		}

		t.origin = gt.xform_inv(newpos + base_position);

		multimesh->set_instance_transform(i, t);

		if (prefab != Ref<PackedScene>()) {
			Node* n = prefab->instance();
			prefabs->add_child(n);
			n->set_owner(get_tree()->get_edited_scene_root());
			Spatial* p = cast_to<Spatial>(n);
			if (p) {
				p->set_transform(t);
			}
		}
	}
}

void PropArea::find_path()
{
	if (path)
		return;
	auto children = get_children();
	for (int i = 0; i < children.size(); ++i) {
		if (cast_to<Node>(children[i])->get_class() == String("Path")) {
			path = cast_to<Path>(children[i]);
			path->connect("curve_changed", this, "_on_path_curve_changed");
			path->connect("curve_changed", this, "update_configuration_warning");
			path->set_notify_transform(true);
			return;
		}
	}
	path = nullptr;
}

void PropArea::update_curve()
{
	if (curve2d == Ref<Curve2D>()) {
		PRINT_ERROR("Curve2D reference not found!", "PropArea::update_curve");
		return;
	}
	curve2d->clear_points();
	auto curve = path->get_curve();
	auto t = path->get_transform();
	for (int i = 0; i < curve->get_point_count(); ++i) {
		Vector3 pos3 = t.xform(curve->get_point_position(i));
		Vector3 in3 = t.xform(curve->get_point_in(i));
		Vector3 out3 = t.xform(curve->get_point_out(i));
		curve2d->add_point(Vector2(pos3.x, pos3.z), Vector2(in3.x, in3.z), Vector2(out3.x, out3.z));
	}
}

Spatial* PropArea::get_prefab_root()
{
	if (!has_node("Prefabs")) {
		Spatial* p = Spatial::_new();
		add_child(p);
		p->set_owner(get_tree()->get_edited_scene_root());
		p->set_name("Prefabs");
	}
	return cast_to<Spatial>(get_node("Prefabs"));
}

void PropArea::clear_prefabs()
{
	Spatial* prefabs = get_prefab_root();
	Array children = prefabs->get_children();
	for (int i = 0; i < children.size(); ++i) {
		static_cast<Node*>(children[i])->queue_free();
	}
}
