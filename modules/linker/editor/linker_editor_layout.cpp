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
			//	emit_signal("inspect_links_request", links, Vector2(p_point));
			connect_next->dropped(drop_data.links[0], p_point);
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
			Vector<LinkConnection *> _connections = connections_map[source_link][destination_link];
			for (int i = 0; i < _connections.size(); i++) {
				if (VariantUtilityFunctions::is_instance_valid(_connections[i])) {
					if (_connections[i]->connection_type == p_connection_type) {
						return _connections[i];
					}
				}
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
		connections_map[source_link] = HashMap<Ref<LinkerLink>, Vector<LinkConnection *>>();
	}
	connections_map[source_link][destination_link].append(connection);
	return connection;
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
	connect_next = memnew(ConnectNext);
	add_child(connect_next);
}

void ResultTree::add_source_info(const PropertyInfo &p_info) {
	source_info.push_back(p_info);
}

void ResultTree::set_case_sensitive(bool p_case_sensitive) {
	if (p_case_sensitive) {
		//	search_flags |= SEARCH_CASE_SENSITIVE;
	} else {
		// search_flags &= ~SEARCH_CASE_SENSITIVE;
	}
}

void ResultTree::set_show_hierarchy(bool p_show_hierarchy) {
	if (p_show_hierarchy) {
		// search_flags |= SEARCH_SHOW_HIERARCHY;
	} else {
		// search_flags &= ~SEARCH_SHOW_HIERARCHY;
	}
}

void ResultTree::update_results(const String &p_search_term) {
	search_term = p_search_term;
	update_results();
}

void ResultTree::update_results() {
	clear();
	ERR_PRINT(search_term);
	List<MethodInfo> m;
	ClassDB::get_method_list(search_term, &m);
	for (List<MethodInfo>::Element *E = m.front(); E; E = E->next()) {
		if (E->get().name.begins_with("get_")) {
			print_line(E->get().name);
		}
	}
}

void ConnectNext::source_flag_pressed(int p_flag) {
	int flag_pressed = 1 << p_flag;
	if (Input::get_singleton()->is_key_pressed(Key::CTRL)) {
		source_flags = SOURCE_NONE;
	}
	source_flags = static_cast<ConnectNext::SourceFlags>(static_cast<int>(source_flags) ^ flag_pressed);

	source_menu->get_popup()->set_item_checked(0, source_flags & SOURCE_BUILT_IN);
	source_menu->get_popup()->set_item_checked(1, source_flags & SOURCE_HOST);
	source_menu->get_popup()->set_item_checked(2, source_flags & SOURCE_LINK);

	if (source_flags == SOURCE_NONE) {
		source_menu->set_text("_"); // place holder art
	} else if (source_flags == SOURCE_BUILT_IN) {
		source_menu->set_text("\\/"); // place holder art
	} else if (source_flags == SOURCE_HOST) {
		source_menu->set_text("|>"); // place holder art
	} else if (source_flags == SOURCE_LINK) {
		source_menu->set_text("->"); // place holder art
	} else {
		source_menu->set_text("*");
	}
}

void ConnectNext::popup_closed() {
	ERR_PRINT("update_results");
}

void ConnectNext::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			close_button->set_icon(Control::get_theme_icon(SNAME("Close"), EditorStringName(EditorIcons)));
		} break;
		case NOTIFICATION_MOUSE_ENTER: {
			mouse_inside = true;
			queue_redraw();
			break;
		}
		case NOTIFICATION_MOUSE_EXIT: {
			mouse_inside = false;
			queue_redraw();
			break;
		}
		case NOTIFICATION_DRAW: {
			_draw_debug();
			break;
		}
	}
}

void ConnectNext::_draw_debug() {
	if (debug_mouse_inside && mouse_inside) {
		draw_rect(Rect2(Vector2(), get_size()), Color(0.5, 1, 1, 0.3), false, 1.0);
	}
}

void ConnectNext::dropped(Ref<LinkerLink> p_link, const Point2 &p_point) {
	popup(p_point);
	// get link return data and find some stuff
}

void ConnectNext::popup(const Vector2 &p_pos) {
	set_visible(true);
	set_position(p_pos);
	set_size(Vector2(100, 100));
}

void ConnectNext::close() {
	// callable_mp(results_tree, &ResultTree::clear).call_deferred(); // Wait for the Tree's mouse event propagation.
	set_visible(false);
}

ConnectNext::ConnectNext() {
	menu_bar = memnew(HBoxContainer);
	add_child(menu_bar);

	source_menu = memnew(MenuButton);
	menu_bar->add_child(source_menu);
	source_menu->set_text("*");
	source_menu->get_popup()->add_radio_check_item(TTR("from built-in")); // SOURCE_BUILT_IN
	source_menu->get_popup()->add_radio_check_item(TTR("from script")); // SOURCE_HOST
	source_menu->get_popup()->add_radio_check_item(TTR("from link")); // SOURCE_LINK
	source_menu->get_popup()->set_hide_on_checkable_item_selection(false);
	source_menu->get_popup()->connect("id_pressed", callable_mp(this, &ConnectNext::source_flag_pressed));
	source_menu->get_popup()->connect("popup_hide", callable_mp(this, &ConnectNext::popup_closed)); // update results

	menu_bar->set_alignment(BoxContainer::ALIGNMENT_END);
	close_button = memnew(Button);
	menu_bar->add_child(close_button);
	close_button->set_flat(true);
	close_button->connect("pressed", callable_mp(this, &ConnectNext::close));

	search_tekst = memnew(LineEdit);
	add_child(search_tekst);
	search_tekst->set_tooltip_text(TTR("Enter \" \" to show all filtered options\nEnter \".\" to show all filtered methods, operators and constructors\nUse CTRL_KEY to drop property setters"));
	search_tekst->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	// search_box->connect("text_changed", callable_mp(this, &VisualScriptPropertySelector::_update_results_s));
	// search_box->connect("gui_input", callable_mp(this, &VisualScriptPropertySelector::_sbox_input));

	results_tree = memnew(ResultTree);
	add_child(results_tree);
	results_tree->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	results_tree->set_columns(2);
	results_tree->set_column_title(0, TTR("Name"));
	results_tree->set_column_clip_content(0, true);
	results_tree->set_column_title(1, TTR("Member Type"));
	results_tree->set_column_expand(1, false);
	results_tree->set_column_custom_minimum_width(1, 150 * EDSCALE);
	results_tree->set_column_clip_content(1, true);
	results_tree->set_custom_minimum_size(Size2(0, 100) * EDSCALE);
	results_tree->set_hide_root(true);
	results_tree->set_select_mode(Tree::SELECT_ROW);
	results_tree->update_results("Node");
	// results_tree->connect("item_activated", callable_mp(this, &EditorHelpSearch::_confirmed));
	// results_tree->connect("item_selected", callable_mp((BaseButton *)get_ok_button(), &BaseButton::set_disabled).bind(false));

	close();
}

ConnectNext::~ConnectNext() {
}
