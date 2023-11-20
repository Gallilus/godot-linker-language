#include "linker_graph.h"

static Node *_find_script_node(Node *p_edited_scene, Node *p_current_node, const Ref<Script> &script) {
	if (p_edited_scene != p_current_node && p_current_node->get_owner() != p_edited_scene) {
		return nullptr;
	}

	Ref<Script> scr = p_current_node->get_script();

	if (scr.is_valid() && scr == script) {
		return p_current_node;
	}

	for (int i = 0; i < p_current_node->get_child_count(); i++) {
		Node *n = _find_script_node(p_edited_scene, p_current_node->get_child(i), script);
		if (n) {
			return n;
		}
	}

	return nullptr;
}

static void _get_node_data(Node *p_node, Ref<LinkerSceneRefrence> p_node_info, Node *p_scripted_node = nullptr) {
	p_node_info->set_node_class_name(p_node->get_class_name());
	p_node_info->set_node_name(p_node->get_name());
	p_node_info->set_node_scene_path(EditorInterface::get_singleton()->get_edited_scene_root()->get_path_to(p_node));
	p_node_info->set_node_script_file_path(p_node->get_scene_file_path());
	p_node_info->set_node_scene_relative_path(StringName(p_scripted_node->get_path_to(p_node)));
}

void LinkerGraph::_bind_methods() {
}

void LinkerGraph::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			break;
		}
			/*case NOTIFICATION_PRE_SORT_CHILDREN: {
				// make sure all link controlers are resized

				// and trigger recalculation of Graph positions
				break;
			}
			case NOTIFICATION_SORT_CHILDREN: {
				// reposiotn all link controlers
				break;
			}*/
	}
}

void LinkerGraph::gui_input(const Ref<InputEvent> &p_event) {
	ERR_FAIL_COND(p_event.is_null());
}

bool LinkerGraph::can_drop_data(const Point2 &p_point, const Variant &p_data) const {
	Dictionary d = p_data;
	if (d.has("type") &&
			(String(d["type"]) == "obj_property" ||
					String(d["type"]) == "resource" ||
					String(d["type"]) == "files" ||
					String(d["type"]) == "nodes")) {
		if (String(d["type"]) == "obj_property") {
			// optional show hint
		}
		if (String(d["type"]) == "nodes") {
			// optional show hint
		}
		if (String(d["type"]) == "visual_script_variable_drag") {
			// optional show hint
		}
		return true;
	}
	return false;
}

void LinkerGraph::drop_data(const Point2 &p_point, const Variant &p_data) {
	Dictionary d = p_data;
	if (!d.has("type")) {
		ERR_PRINT("LinkerGraph can not handle drop data:" + String(p_data));
		return;
	}
	//ERR_PRINT("can_drop_data " + String(p_data));

	Node *scripted_node = _find_script_node(get_tree()->get_edited_scene_root(), get_tree()->get_edited_scene_root(), script);
	String this_script_path = script->get_path();

	DropData drop_data;

	drop_data.ctrl_drop = Input::get_singleton()->is_key_pressed(Key::CTRL);
	drop_data.shift_drop = Input::get_singleton()->is_key_pressed(Key::SHIFT);

	if (d.has("object")) {
		Variant var = d["object"];
		if (var) {
			drop_data.object_type = var.get_type();
		}
		if (drop_data.object_type == Variant::Type::OBJECT) {
			Object *obj = Object::cast_to<Object>(d["object"]);
			ERR_FAIL_COND(!obj);
			drop_data.object_class_name = obj->get_class_name();
			Node *node = Object::cast_to<Node>(d["object"]);
			if (node) {
				Ref<LinkerSceneRefrence> node_info = memnew(LinkerSceneRefrence);
				node_info->set_host(script.ptr());
				_get_node_data(node, node_info, scripted_node);
				drop_data.nodes.append(node_info);
			}
		}
	}

	if (d.has("property")) {
		drop_data.property_name = d["property"];
	}

	if (d.has("value")) {
		drop_data.value = d["value"];
	}

	if (d.has("nodes")) {
		Array nodes = d["nodes"];
		for (int i = 0; i < nodes.size(); i++) {
			NodePath node_path = nodes[i];
			Node *node = get_node(node_path);
			if (node) {
				Ref<LinkerSceneRefrence> node_info = memnew(LinkerSceneRefrence);
				node_info->set_host(script.ptr());
				_get_node_data(node, node_info, scripted_node);
				drop_data.nodes.append(node_info);
			}
		}
	}

	if (String(d["type"]) == "obj_property") {
		if (drop_data.nodes.size() == 1) {
			StringName rel_path = drop_data.nodes[0]->get_node_scene_relative_path();
			Ref<LinkerSceneRefrence> node_info = drop_data.nodes[0];
			script->add_link(node_info);
			node_info = script->get_scene_refrence(rel_path);
			Ref<LinkerIndexGet> index_get = memnew(LinkerIndexGet);
			index_get->set_index(drop_data.property_name);
			index_get->set_source(node_info);
			script->add_link(index_get);
		}

		// create a property getter

		// if droped on graphical interface overwrite @to do in graphical interface
	}

	if (String(d["type"]) == "nodes") {
		if (drop_data.nodes.size() == 1) {
			script->add_link(drop_data.nodes[0]);
			// set popup inspector for keyed and or select properties or methods
			return;
		}
		for (int i = 0; i < drop_data.nodes.size(); i++) {
			script->add_link(drop_data.nodes[i]);
		}
	}
}

void LinkerGraph::update_graph() {
	if (!script.is_valid()) {
		return;
	}

	for (int i = 0; i < unsorted_nodes->get_child_count(); i++) {
		Node *child = unsorted_nodes->get_child(i);
		child->queue_free();
	}
	// if (unsorted_nodes) {
	// 	unsorted_nodes->queue_free();
	// 	unsorted_nodes = nullptr;
	// }
	// unsorted_nodes = memnew(VBoxContainer);
	// //unsorted_nodes->set_columns(1);
	// unsorted_nodes->set_custom_minimum_size(Size2(28, 28));
	// unsorted_nodes->set_anchors_preset(Control::PRESET_CENTER_LEFT);
	// add_child(unsorted_nodes);

	TypedArray<LinkerSceneRefrence> scene_refs = script->get_scene_refrences();

	for (int i = 0; i < scene_refs.size(); i++) {
		Ref<LinkerSceneRefrence> node_info = scene_refs[i];

		LinkControler *controler = memnew(LinkControler);
		controler->set_link(node_info);
		unsorted_nodes->add_child(controler);
	}
}

LinkerGraph::LinkerGraph() {
	set_v_size_flags(SIZE_EXPAND_FILL);
	set_h_size_flags(SIZE_EXPAND_FILL);
	set_focus_mode(Control::FOCUS_ALL);

	ScrollContainer *scroll = memnew(ScrollContainer);
	add_child(scroll);
	scroll->set_custom_minimum_size(Size2(35, 100));
	scroll->set_anchors_preset(PRESET_CENTER_LEFT);
	scroll->set_anchor(SIDE_TOP, 1.0 / 3.0);
	scroll->set_anchor(SIDE_BOTTOM, 1.0 - 1.0 / 3.0);
	scroll->set_horizontal_scroll_mode(ScrollContainer::SCROLL_MODE_DISABLED);

	unsorted_nodes = memnew(VBoxContainer);
	scroll->add_child(unsorted_nodes);
}
