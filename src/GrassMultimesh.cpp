#include <SceneTree.hpp>
#include <MeshInstance.hpp>
#include <MultiMeshInstance.hpp>
#include <Transform.hpp>

#include "GrassMultimesh.hpp"

using namespace godot;

void GrassMultimesh::_register_methods()
{
    register_property("multimesh", &GrassMultimesh::multimesh, Ref<MultiMesh>());
    register_property("mesh_node_name", &GrassMultimesh::mesh_node_name, String("Grass001"));
    register_property("cast_shadow", &GrassMultimesh::cast_shadow, false);

    register_method("_ready", &GrassMultimesh::_ready);
}

void GrassMultimesh::_init()
{
    mesh_node_name = String("Grass001");
    transforms = PoolRealArray();
}

void GrassMultimesh::_ready()
{
    recurse_tree(get_tree()->get_current_scene());
    MultiMeshInstance* multi_mesh_instance = MultiMeshInstance::_new();
    multi_mesh_instance->set_cast_shadows_setting((int64_t)cast_shadow);
    multimesh->set_instance_count(current_instance);
    //multimesh->set_as_bulk_array(transforms);
    for (int64_t i = 0; i < current_instance; ++i) {
        multimesh->set_instance_transform(i, transform_vector[i]);
    }
    add_child(multi_mesh_instance);
    multi_mesh_instance->set_multimesh(multimesh);
}

void GrassMultimesh::recurse_tree(Node* root)
{
    Array children = root->get_children();
    if (!children.empty()) {
        for (int64_t i = 0; i < children.size(); ++i) {
            recurse_tree(cast_to<Node>(children[i]));
        }
    }

    if (root->get_name() == mesh_node_name) {
        MeshInstance* mesh_instance = cast_to<MeshInstance>(root);
        append_transform(mesh_instance->get_global_transform());
        transform_vector.push_back(mesh_instance->get_global_transform());
        ++current_instance;
        mesh_instance->queue_free();
    }
}

void GrassMultimesh::append_transform(const Transform& t)
{
    transforms.append(t.basis[0].x);
    transforms.append(t.basis[0].y);
    transforms.append(t.basis[0].z);
    transforms.append(t.basis[1].x);
    transforms.append(t.basis[1].y);
    transforms.append(t.basis[1].z);
    transforms.append(t.basis[2].x);
    transforms.append(t.basis[2].y);
    transforms.append(t.basis[2].z);
    transforms.append(t.origin.x);
    transforms.append(t.origin.y);
    transforms.append(t.origin.z);
}