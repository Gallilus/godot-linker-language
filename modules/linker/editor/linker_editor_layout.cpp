#include "linker_editor_layout.h"
#include "editor_graph.h"
#include "link_connection.h"
#include "link_controler.h"

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

void LinkerEditorLayout::_bind_methods() {
	ADD_SIGNAL(MethodInfo("inspect_links_request", PropertyInfo(Variant::ARRAY, "links"), PropertyInfo(Variant::VECTOR2, "glob_pos")));
}

LinkControler *LinkerEditorLayout::make_link_controler(Ref<LinkerLink> p_link) {
	LinkControler *controler = memnew(LinkControler);
	link_contorlers[p_link] = controler;
	controler->set_link(p_link);
	add_child(controler);
	return controler;
}

void LinkerEditorLayout::gui_input(const Ref<InputEvent> &p_event) {
	Ref<InputEventMouseButton> mb = p_event;
	if (mb.is_valid()) {
		if (mb->is_double_click()) {
			// popup menu
			ERR_PRINT("create popup");
			get_viewport()->set_input_as_handled();
		}
	}
}

bool LinkerEditorLayout::can_drop_data(const Point2 &p_point, const Variant &p_data) const {
	// Check position if it is relevant to you
	// Otherwise, just check data
	Dictionary d = p_data;
	if (d.has("type") &&
			(String(d["type"]) == "linker_link" ||
					String(d["type"]) == "obj_property" ||
					String(d["type"]) == "resource" ||
					String(d["type"]) == "files" ||
					String(d["type"]) == "nodes" ||
					String(d["type"]) == "variable" ||
					String(d["type"]) == "method" ||
					String(d["type"]) == "func_return" ||
					String(d["type"]) == "func_arg")) {
		return true;
	}
	return false;
}

void LinkerEditorLayout::drop_data(const Point2 &p_point, const Variant &p_data) {
	Node *scripted_node = _find_script_node(get_tree()->get_edited_scene_root(), get_tree()->get_edited_scene_root(), script);
	String this_script_path = script->get_path();

	DropData drop_data;
	drop_data.ctrl_drop = Input::get_singleton()->is_key_pressed(Key::CTRL);
	drop_data.shift_drop = Input::get_singleton()->is_key_pressed(Key::SHIFT);

	Dictionary d = p_data;

	if (d.has("link_idx")) {
		Ref<LinkerLink> drag_link = script->get_link(d["link_idx"]);
		drop_data.links.append(drag_link);
	}

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
		// check if value can be cast to PropertyInfo
		// check if value can be cast to MethodInfo
	}

	if (d.has("nodes")) {
		Array nodes = d["nodes"];
		for (int i = 0; i < nodes.size(); i++) {
			NodePath node_path = nodes[i];
			Node *node = get_node(node_path);
			if (node) {
				Ref<LinkerSceneRefrence> node_info = memnew(LinkerSceneRefrence);
				_get_node_data(node, node_info, scripted_node);
				drop_data.nodes.append(node_info);
			}
		}
	}

	if (String(d["type"]) == "linker_link") {
		if (drop_data.links.size() == 1) {
			Array links;
			links.append(d["link_idx"]);
			// emit inspect link+pos
			emit_signal("inspect_links_request", links, Vector2(p_point));

			// editor inspector for return type
			// editor inspector for next function
			// higlight compatible links
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
	}

	if (String(d["type"]) == "nodes") {
		for (int i = 0; i < drop_data.nodes.size(); i++) {
			script->add_link(drop_data.nodes[i]);
		}
	}

	if (String(d["type"]) == "method") {
		MethodInfo method_info = MethodInfo::from_dict(drop_data.value);
		Ref<LinkerLink> ll = script->get_function_link(method_info.name);
		script->add_link(ll);
	}

	if (String(d["type"]) == "func_return") {
		ERR_PRINT("droped func_return");
	}

	if (String(d["type"]) == "func_arg") {
		ERR_PRINT("droped func_arg");
	}

	if (String(d["type"]) == "variable") {
		ERR_PRINT("droped variable");
	}
}

LinkControler *LinkerEditorLayout::get_link_source_controler(Ref<LinkerLink> p_link) {
	if (!p_link.is_valid()) {
		return nullptr;
	}
	if (p_link->has_source()) {
		return get_link_controler(p_link->get_source());
	}
	return nullptr;
}

LinkControler *LinkerEditorLayout::get_link_controler(Ref<LinkerLink> p_link) {
	if (!p_link.is_valid()) {
		return nullptr;
	}
	if (link_contorlers.has(p_link)) {
		if (VariantUtilityFunctions::is_instance_valid(link_contorlers[p_link])) {
			return link_contorlers[p_link];
		}
	}
	if (p_link->controler_at_source()) {
		for (const KeyValue<Ref<LinkerLink>, LinkControler *> &E : link_contorlers) {
			if (E.key.is_valid() && E.key.ptr() == p_link.ptr()) {
				if (VariantUtilityFunctions::is_instance_valid(E.value)) {
					link_contorlers[p_link] = E.value;
					return link_contorlers[p_link];
				}
			}
		}
	}
	return make_link_controler(p_link);
}

LinkConnection *LinkerEditorLayout::get_link_connection(Ref<LinkerLink> source_link, Ref<LinkerLink> destination_link, int p_connection_type) {
	if (!source_link.is_valid() || !destination_link.is_valid()) {
		return nullptr;
	}
	// get connection
	if (connections_map.has(source_link)) {
		if (connections_map[source_link].has(destination_link)) {
			if (VariantUtilityFunctions::is_instance_valid(connections_map[source_link][destination_link])) {
				if (connections_map[source_link][destination_link]->connection_type != p_connection_type) {
					ERR_PRINT("connection type overriden");
					connections_map[source_link][destination_link]->connection_type = static_cast<LinkConnection::ConnectionType>(p_connection_type);
				}
				return connections_map[source_link][destination_link];
			}
		}
	}

	// create connection
	LinkConnection *connection = memnew(LinkConnection);
	add_child(connection);
	connection->set_start(get_link_controler(source_link));
	connection->set_end(get_link_controler(destination_link));
	connection->connection_type = static_cast<LinkConnection::ConnectionType>(p_connection_type);

	// store connection
	if (!connections_map.has(source_link)) {
		connections_map[source_link] = HashMap<Ref<LinkerLink>, LinkConnection *>();
	} else if (connections_map[source_link].has(destination_link)) {
		connections_map[source_link][destination_link]->queue_free();
	}
	connections_map[source_link][destination_link] = connection;
	return connections_map[source_link][destination_link];
}

void LinkerEditorLayout::update_graph() {
	EditorGraph graph;
	_graph = &graph;
	if (!script.is_valid()) {
		return;
	}

	{ // clear layout
		for (const KeyValue<Ref<LinkerLink>, LinkControler *> &E : link_contorlers) {
			if (E.value) {
				E.value->set_visible(false);
			}
		}
	}

	script->for_every_link(callable_mp(this, &LinkerEditorLayout::add_link));
	script->for_every_pulled(callable_mp(this, &LinkerEditorLayout::add_pull_connection));
	script->for_every_sequenced(callable_mp(this, &LinkerEditorLayout::add_sequence_connection));

	for (const KeyValue<Ref<LinkerLink>, Vector2> &E : graph.get_linker_link_positions()) {
		Ref<LinkerLink> link = E.key;
		if (link.is_valid()) {
			LinkControler *controler = get_link_controler(link);
			controler->set_visible(true);
			Ref<Tween> tween = create_tween();
			tween->tween_property(controler, NodePath("position"), E.value, 0.5);
		}
	}
}

void LinkerEditorLayout::add_link(Ref<LinkerLink> p_link) {
	_graph->add_vertex(p_link);
	get_link_controler(p_link); // create the controler
}

void LinkerEditorLayout::add_pull_connection(Ref<LinkerLink> pulled_link, Ref<LinkerLink> owner_link) {
	_graph->add_pull_edge(pulled_link, owner_link);
	get_link_connection(pulled_link, owner_link, LinkConnection::CONNECTION_TYPE_SOURCE); // create the connection
}

void LinkerEditorLayout::add_sequence_connection(Ref<LinkerLink> source_link, Ref<LinkerLink> destination_link) {
	_graph->add_sequence_edge(source_link, destination_link);
	get_link_connection(source_link, destination_link, LinkConnection::CONNECTION_TYPE_SEQUENCE); // create the connection
}

LinkerEditorLayout::LinkerEditorLayout() {
	set_v_size_flags(SIZE_EXPAND_FILL);
	set_h_size_flags(SIZE_EXPAND_FILL);
	set_focus_mode(Control::FOCUS_ALL);
}
