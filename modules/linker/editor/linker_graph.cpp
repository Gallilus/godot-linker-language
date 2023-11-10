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

static void _get_node_data(Node *p_node, LinkerScript::NodeInfo &p_node_info, Node *p_scripted_node = nullptr) {
	p_node_info.node_class_name = p_node->get_class_name();
	p_node_info.node_name = p_node->get_name();
	p_node_info.node_scene_path = EditorInterface::get_singleton()->get_edited_scene_root()->get_path_to(p_node);
	p_node_info.node_script_file_path = p_node->get_scene_file_path();
	p_node_info.node_scene_relative_path = StringName(p_scripted_node->get_path_to(p_node));
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
				LinkerScript::NodeInfo node_info;
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
				LinkerScript::NodeInfo node_info;
				_get_node_data(node, node_info, scripted_node);
				drop_data.nodes.append(node_info);
			}
		}
	}

	if (String(d["type"]) == "obj_property") {
		if (drop_data.nodes.size() == 1) {
			script->add_scene_refrence(drop_data.nodes[0]);
		}

		// create a property getter

		// if droped on graphical interface overwrite @to do in graphical interface
	}

	if (String(d["type"]) == "nodes") {
		if (drop_data.nodes.size() == 1) {
			script->add_scene_refrence(drop_data.nodes[0]);
			// set popup inspector for keyed and or select properties or methods
			return;
		}
		for (int i = 0; i < drop_data.nodes.size(); i++) {
			script->add_scene_refrence(drop_data.nodes[i]);
		}
	}

	//	emit_signal(SNAME("changed"));

	//				List<MethodInfo> methods;
	//				ClassDB::get_method_list(obj->get_class_name(), &methods, true, true);
	//				for (List<MethodInfo>::Element *E = methods.front(); E; E = E->next()) {

	/*//	ERR_PRINT("========================================");
	List<MethodInfo> methods;
	p_data.get_method_list(&methods);
	for (List<MethodInfo>::Element *E = methods.front(); E; E = E->next()) {
		// ERR_PRINT(" " + String(E->get().name));
	}
	//	ERR_PRINT("========================================");
	List<PropertyInfo> properties;
	p_data.get_property_list(&properties);
	for (List<PropertyInfo>::Element *E = properties.front(); E; E = E->next()) {
		//ERR_PRINT(" " + String(E->get().name));
	}
	//	ERR_PRINT("***************************************");
	//List<MethodInfo> signals;*/
}

void LinkerGraph::update_graph() {
	if (!script.is_valid()) {
		return;
	}
	if (unsorted_nodes) {
		remove_child(unsorted_nodes);
		memdelete(unsorted_nodes);
		unsorted_nodes = nullptr;
	}
	unsorted_nodes = memnew(GridContainer);
	add_child(unsorted_nodes);

	Dictionary scene_refs = script->get_scene_refrences();

	for (int i = 0; i < scene_refs.keys().size(); i++) {
		Variant value = scene_refs[scene_refs.keys()[i]];
		LinkerScript::NodeInfo node_info = LinkerScript::NodeInfo::from_dict(value);
		LinkerNode *node = memnew(LinkerNode);
		unsorted_nodes->add_child(node);
		node->set_tooltip_text(node_info.node_name);
		node->set_icon(EditorNode::get_singleton()->get_object_icon(this, node_info.node_class_name));
		node->connect("pressed", callable_mp(script.ptr(), &LinkerScript::remove_scene_refrence).bind(node_info.node_scene_relative_path), CONNECT_DEFERRED);
	}
}

LinkerGraph::LinkerGraph() {
	set_v_size_flags(SIZE_EXPAND_FILL);
	set_h_size_flags(SIZE_EXPAND_FILL);
	set_focus_mode(Control::FOCUS_ALL);
}
