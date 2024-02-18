#include "linker_scene_refrence.h"
#include "linker_script.h"

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

void LinkerSceneRefrence::_initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_stack_size) {
	LinkerSceneRefrenceInstance *instance = static_cast<LinkerSceneRefrenceInstance *>(link);

	instance->host = p_host;
	instance->index = index;

	instance->pull_count = pull_links.size();
	instance->push_count = push_links.size();

	if (source_link.is_valid()) {
		instance->source_link = source_link->get_instance(p_host, p_stack_size);
	}

	for (int i = 0; i < instance->pull_count; i++) {
		LinkerLinkInstance *_link = pull_links[i]->get_instance(p_host, p_stack_size);
		if (_link) {
			instance->pull_links.push_back(_link);
		} else {
			ERR_PRINT(String(pull_links[i]->get_class_name()) + ": instance is null");
		}
	}

	for (int i = 0; i < instance->push_count; i++) {
		LinkerLinkInstance *_link = push_links[i]->get_instance(p_host, p_stack_size);
		if (_link) {
			instance->push_links.push_back(_link);
		} else {
			ERR_PRINT(String(push_links[i]->get_class_name()) + ": instance is null");
		}
	}

	instance->node_path = NodePath(node_scene_relative_path);
}

Variant LinkerSceneRefrence::get_placeholder_value() const {
	return EditorInterface::get_singleton()->get_edited_scene_root()->get_node(node_scene_path);
}

Dictionary LinkerSceneRefrence::get_placeholder_info() const {
	// if classname == property name object is class
	Dictionary d;
	d["type"] = "PropertyInfo";
	PropertyInfo pi;
	pi.type = Variant::OBJECT;
	pi.class_name = node_class_name;
	pi.hint_string = node_script_file_path;
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

bool LinkerSceneRefrence::can_drop_on_value(Ref<LinkerLink> drag_link) const {
	print_error(String(drag_link->get_class_name()) + " can take value of " + String(get_class_name()));
	return true;
}

LinkerLinkInstance *LinkerSceneRefrence::get_instance(LinkerScriptInstance *p_host, int p_stack_size) {
	if (!link_instances.has(p_stack_size)) {
		LinkerSceneRefrenceInstance *instance = new LinkerSceneRefrenceInstance();
		link_instances.insert(p_stack_size, p_host->add_link_instance(instance));
		_initialize_instance(link_instances[p_stack_size], p_host, p_stack_size);
	}
	return link_instances[p_stack_size];
}

////////////////////////////////////////////////////////////////////////
//////////////////////////////  INSTANCE  //////////////////////////////
////////////////////////////////////////////////////////////////////////

int LinkerSceneRefrenceInstance::_step(StartMode p_start_mode, Callable::CallError &r_error, String &r_error_str) {
	Node *owner_node = Object::cast_to<Node>(host->get_owner());
	value = Variant(owner_node->get_node(node_path));
	return STEP_COMPLETE;
}
