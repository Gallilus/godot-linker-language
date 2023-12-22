#include "linker_scene_refrence.h"

void LinkerSceneRefrence::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_node_class_name", "node_class_name"), &LinkerSceneRefrence::set_node_class_name);
	ClassDB::bind_method(D_METHOD("get_node_class_name"), &LinkerSceneRefrence::get_node_class_name);
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "node_class_name"), "set_node_class_name", "get_node_class_name");
	ClassDB::bind_method(D_METHOD("set_node_name", "node_name"), &LinkerSceneRefrence::set_node_name);
	ClassDB::bind_method(D_METHOD("get_node_name"), &LinkerSceneRefrence::get_node_name);
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "node_name"), "set_node_name", "get_node_name");
	ClassDB::bind_method(D_METHOD("set_node_script_file_path", "node_script_file_path"), &LinkerSceneRefrence::set_node_script_file_path);
	ClassDB::bind_method(D_METHOD("get_node_script_file_path"), &LinkerSceneRefrence::get_node_script_file_path);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "node_script_file_path"), "set_node_script_file_path", "get_node_script_file_path");
	ClassDB::bind_method(D_METHOD("set_node_scene_path", "node_scene_path"), &LinkerSceneRefrence::set_node_scene_path);
	ClassDB::bind_method(D_METHOD("get_node_scene_path"), &LinkerSceneRefrence::get_node_scene_path);
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "node_scene_path"), "set_node_scene_path", "get_node_scene_path");
	ClassDB::bind_method(D_METHOD("set_node_scene_relative_path", "node_scene_relative_path"), &LinkerSceneRefrence::set_node_scene_relative_path);
	ClassDB::bind_method(D_METHOD("get_node_scene_relative_path"), &LinkerSceneRefrence::get_node_scene_relative_path);
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "node_scene_relative_path"), "set_node_scene_relative_path", "get_node_scene_relative_path");
}

int LinkerSceneRefrence::get_arg_count() const {
	return 0;
}

PropertyInfo LinkerSceneRefrence::get_arg_info(int p_idx) const {
	return PropertyInfo();
}

int LinkerSceneRefrence::get_default_arg_count() const {
	return 0;
}

Variant LinkerSceneRefrence::get_default_arg(int p_idx) const {
	return Variant();
}

PropertyInfo LinkerSceneRefrence::get_output_info() const {
	PropertyInfo pi;
	pi.type = Variant::OBJECT;
	pi.name = node_name;
	pi.class_name = node_class_name;
	return pi;
}

Ref<Texture2D> LinkerSceneRefrence::get_icon() const {
	return EditorNode::get_singleton()->get_class_icon(node_class_name);
}

void LinkerSceneRefrence::set_refrence(Node *p_ref_node, Node *p_scripted_node) {
	node_class_name = p_ref_node->get_class_name();
	node_name = p_ref_node->get_name();
	node_script_file_path = p_ref_node->get_scene_file_path();
	node_scene_path = EditorInterface::get_singleton()->get_edited_scene_root()->get_path_to(p_ref_node);
	node_scene_relative_path = StringName(p_scripted_node->get_path_to(p_ref_node));
}