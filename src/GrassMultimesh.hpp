#pragma once

#include <vector>

#include <Godot.hpp>
#include <Node.hpp>
#include <MultiMesh.hpp>
#include <Transform.hpp>

namespace godot {
    class MeshInstance;
    class Transform;
}

class GrassMultimesh : public godot::Node {
    GODOT_CLASS(GrassMultimesh, godot::Node)

public:
    static void _register_methods();
    void _init();

    void _ready();

private:
    void recurse_tree(godot::Node* root);
    void append_transform(const godot::Transform& t);

    godot::Ref<godot::MultiMesh> multimesh;
    godot::String mesh_node_name;
    bool cast_shadow;

    godot::PoolRealArray transforms;
    std::vector<godot::Transform> transform_vector;
    int64_t current_instance = 0;
};