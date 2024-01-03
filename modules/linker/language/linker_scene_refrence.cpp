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

Variant LinkerSceneRefrence::get_placeholder_value() const {
	// get variant from property info
	return Variant();
}

Dictionary LinkerSceneRefrence::get_placeholder_info() const {
	Dictionary d;
	d["type"] = "PropertyInfo";

	PropertyInfo pi;
	pi.type = Variant::OBJECT;
	pi.name = node_name;
	pi.class_name = node_class_name;
	d["value"] = Dictionary(pi);

	return d;
}

void LinkerSceneRefrence::set_refrence(Node *p_ref_node, Node *p_scripted_node) {
	node_class_name = p_ref_node->get_class_name();
	node_name = p_ref_node->get_name();
	node_script_file_path = p_ref_node->get_scene_file_path();
	node_scene_path = EditorInterface::get_singleton()->get_edited_scene_root()->get_path_to(p_ref_node);
	node_scene_relative_path = StringName(p_scripted_node->get_path_to(p_ref_node));
}

void LinkerSceneRefrence::set_node_scene_relative_path(const StringName &p_node_scene_relative_path) {
	node_scene_relative_path = p_node_scene_relative_path;
	set_index(p_node_scene_relative_path);
}

bool LinkerSceneRefrence::can_drop(Ref<LinkerLink> drag_link) const {
	if (drag_link.is_null()) {
		return false;
	}
	if (drag_link->get_class() == "LinkerIndexGet") {
		return true;
	}
	return false;
}

void LinkerSceneRefrence::drop_data(Ref<LinkerLink> dropped_link) {
	if (dropped_link.is_null()) {
		return;
	}
	dropped_link->set_source(this);
}