#include "linker_editor_layout.h"
// #include "editor/editor_string_names.h"
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
				Ref<LinkerSceneRefrence> node_info = LinkerEditorLayout::create_scenerefrence(node, scripted_node);
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
				Ref<LinkerSceneRefrence> scene_ref = LinkerEditorLayout::create_scenerefrence(node, scripted_node);
				drop_data.nodes.append(scene_ref);
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
			Ref<LinkerSceneRefrence> scene_ref = drop_data.nodes[0];
			script->add_link(scene_ref);
			scene_ref = script->get_scene_refrence(rel_path);
			Ref<LinkerIndexGet> index_get = LinkerEditorLayout::create_index_get(drop_data.property_name, scene_ref);
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

Ref<LinkerLink> LinkerEditorLayout::create_scenerefrence(Node *to_node, Node *p_scripted_node) {
	Ref<LinkerSceneRefrence> scene_refrence;
	scene_refrence.instantiate();
	scene_refrence->set_node_class_name(to_node->get_class_name());
	scene_refrence->set_node_name(to_node->get_name());
	scene_refrence->set_node_scene_path(EditorInterface::get_singleton()->get_edited_scene_root()->get_path_to(to_node));
	scene_refrence->set_node_script_file_path(to_node->get_scene_file_path());
	scene_refrence->set_node_scene_relative_path(StringName(p_scripted_node->get_path_to(to_node)));
	return scene_refrence;
}

Ref<LinkerLink> LinkerEditorLayout::create_index_get(const String &index, const Ref<LinkerLink> &p_source_link) {
	Ref<LinkerIndexGet> index_get;
	index_get.instantiate();
	index_get->set_index(index);
	index_get->set_source(p_source_link);
	return index_get;
}

Ref<LinkerLink> LinkerEditorLayout::create_index_call(const String &index, const Ref<LinkerLink> &p_source_link, const Vector<Ref<LinkerLink>> &p_arguments) {
	Ref<LinkerIndexCall> index_call;
	index_call.instantiate();
	index_call->set_index(index);
	index_call->set_source(p_source_link);
	// add arguments
	return index_call;
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
		search_flags |= SEARCH_CASE_SENSITIVE;
	} else {
		search_flags &= ~SEARCH_CASE_SENSITIVE;
	}
}

void ResultTree::set_show_hierarchy(bool p_show_hierarchy) {
	if (p_show_hierarchy) {
		search_flags |= SEARCH_SHOW_HIERARCHY;
	} else {
		search_flags &= ~SEARCH_SHOW_HIERARCHY;
	}
}

void ResultTree::set_show_setters_getters(bool p_show_setters_getters) {
	if (p_show_setters_getters) {
		search_flags &= ~SEARCH_EXLUDE_FROM_PROPERTIES;
	} else {
		search_flags |= SEARCH_EXLUDE_FROM_PROPERTIES;
	}
}

void ResultTree::update_results(const String &p_search_term) {
	search_term = p_search_term;
	update_results();
}

void ResultTree::update_results() {
	get_root()->clear_children();
	List<MethodInfo> m;
	ClassDB::get_method_list(class_hint, &m, scope_flags & SCOPE_INHERITERS, search_flags & SEARCH_EXLUDE_FROM_PROPERTIES);
	Ref<Texture2D> icon = Control::get_theme_icon(SNAME("MemberMethod"), EditorStringName(EditorIcons));
	for (List<MethodInfo>::Element *E = m.front(); E; E = E->next()) {
		if (search_term != "" && E->get().name.find(search_term) == -1) {
			continue;
		}
		TreeItem *ti = get_root()->create_child();
		ti->set_text(1, E->get().name);
		ti->set_tooltip_text(1, E->get().name);
		ti->set_icon(1, icon);
		ti->set_meta("MethodInfo", Dictionary(E->get()));
		ti->set_meta("type", "MethodInfo");
	}
}

ResultTree::ResultTree() {
	create_item(); // create root
}

void ConnectNext::source_flag_pressed(int p_flag) {
	int flag_pressed = 1 << p_flag;
	if (Input::get_singleton()->is_key_pressed(Key::CTRL)) {
		source_flags = 0;
	}
	source_flags ^= flag_pressed;
	refresh_source_menu();
}

void ConnectNext::refresh_source_menu() {
	source_menu->get_popup()->set_item_checked(0, source_flags & SOURCE_BUILT_IN);
	source_menu->get_popup()->set_item_checked(1, source_flags & SOURCE_HOST);
	source_menu->get_popup()->set_item_checked(2, source_flags & SOURCE_LINK);

	if (source_flags == SOURCE_NONE) {
		source_menu->set_icon(Control::get_theme_icon(SNAME("GuiVisibilityHidden"), EditorStringName(EditorIcons)));
		source_menu->set_text("");
	} else if (source_flags == SOURCE_BUILT_IN) {
		source_menu->set_icon(Control::get_theme_icon(SNAME("ExternalLink"), EditorStringName(EditorIcons)));
	} else if (source_flags == SOURCE_HOST) {
		source_menu->set_icon(Control::get_theme_icon(SNAME("Script"), EditorStringName(EditorIcons)));
	} else if (source_flags == SOURCE_LINK) {
		source_menu->set_icon(Control::get_theme_icon(SNAME("Unlinked"), EditorStringName(EditorIcons)));
	} else {
		source_menu->set_icon(Control::get_theme_icon(SNAME("GuiVisibilityVisible"), EditorStringName(EditorIcons)));
	}
}

void ConnectNext::scope_flag_pressed(int p_flag) {
	int flag_pressed = 1 << p_flag;
	if (Input::get_singleton()->is_key_pressed(Key::CTRL)) {
		scope_flags = 0;
	}
	scope_flags ^= flag_pressed;
	refresh_scope_menu();
}

void ConnectNext::refresh_scope_menu() {
	scope_menu->get_popup()->set_item_checked(0, scope_flags & ResultTree::SCOPE_BASE);
	scope_menu->get_popup()->set_item_checked(1, scope_flags & ResultTree::SCOPE_INHERITERS);
	scope_menu->get_popup()->set_item_checked(2, scope_flags & ResultTree::SCOPE_UNRELATED);

	if (scope_flags == 0) {
		scope_menu->set_icon(Control::get_theme_icon(SNAME("GuiVisibilityHidden"), EditorStringName(EditorIcons)));
	} else if (scope_flags == ResultTree::SCOPE_BASE) {
		scope_menu->set_icon(Control::get_theme_icon(SNAME("CollapseTree"), EditorStringName(EditorIcons)));
	} else if (scope_flags == ResultTree::SCOPE_INHERITERS) {
		scope_menu->set_icon(Control::get_theme_icon(SNAME("FileTree"), EditorStringName(EditorIcons)));
	} else if (scope_flags == ResultTree::SCOPE_UNRELATED) {
		scope_menu->set_icon(Control::get_theme_icon(SNAME("ExpandTree"), EditorStringName(EditorIcons)));
	} else {
		scope_menu->set_icon(Control::get_theme_icon(SNAME("GuiVisibilityVisible"), EditorStringName(EditorIcons)));
	}
}

void ConnectNext::search_flag_pressed(int p_flag) {
	int flag_pressed = 1 << p_flag;
	if (Input::get_singleton()->is_key_pressed(Key::CTRL)) {
		search_flags = 0;
	}
	search_flags ^= flag_pressed;
	refresh_search_menu();
}

void ConnectNext::refresh_search_menu() {
	search_menu->get_popup()->set_item_checked(0, search_flags & ResultTree::SEARCH_CLASSES);
	search_menu->get_popup()->set_item_checked(1, search_flags & ResultTree::SEARCH_CONSTRUCTORS);
	search_menu->get_popup()->set_item_checked(2, search_flags & ResultTree::SEARCH_METHODS);
	search_menu->get_popup()->set_item_checked(3, search_flags & ResultTree::SEARCH_OPERATORS);
	search_menu->get_popup()->set_item_checked(4, search_flags & ResultTree::SEARCH_SIGNALS);
	search_menu->get_popup()->set_item_checked(5, search_flags & ResultTree::SEARCH_CONSTANTS);
	search_menu->get_popup()->set_item_checked(6, search_flags & ResultTree::SEARCH_PROPERTIES);
	search_menu->get_popup()->set_item_checked(7, search_flags & ResultTree::SEARCH_THEME_ITEMS);

	if (search_flags == 0) {
		search_menu->set_icon(Control::get_theme_icon(SNAME("GuiVisibilityHidden"), EditorStringName(EditorIcons)));
	} else if (search_flags == ResultTree::SEARCH_CLASSES) {
		search_menu->set_icon(Control::get_theme_icon(SNAME("Object"), EditorStringName(EditorIcons)));
	} else if (search_flags == ResultTree::SEARCH_CONSTRUCTORS) {
		search_menu->set_icon(Control::get_theme_icon(SNAME("ResourcePreloader"), EditorStringName(EditorIcons)));
	} else if (search_flags == ResultTree::SEARCH_METHODS) {
		search_menu->set_icon(Control::get_theme_icon(SNAME("MemberMethod"), EditorStringName(EditorIcons)));
	} else if (search_flags == ResultTree::SEARCH_OPERATORS) {
		search_menu->set_icon(Control::get_theme_icon(SNAME("Add"), EditorStringName(EditorIcons)));
	} else if (search_flags == ResultTree::SEARCH_SIGNALS) {
		search_menu->set_icon(Control::get_theme_icon(SNAME("MemberSignal"), EditorStringName(EditorIcons)));
	} else if (search_flags == ResultTree::SEARCH_CONSTANTS) {
		search_menu->set_icon(Control::get_theme_icon(SNAME("MemberConstant"), EditorStringName(EditorIcons)));
	} else if (search_flags == ResultTree::SEARCH_PROPERTIES) {
		search_menu->set_icon(Control::get_theme_icon(SNAME("MemberProperty"), EditorStringName(EditorIcons)));
	} else if (search_flags == ResultTree::SEARCH_THEME_ITEMS) {
		search_menu->set_icon(Control::get_theme_icon(SNAME("MemberTheme"), EditorStringName(EditorIcons)));
	} else {
		search_menu->set_icon(Control::get_theme_icon(SNAME("GuiVisibilityVisible"), EditorStringName(EditorIcons)));
	}
}

void ConnectNext::popup_closed() {
	results_tree->search_flags = search_flags;
	results_tree->scope_flags = scope_flags;
	results_tree->update_results();
}

void ConnectNext::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			_update_icons();
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

void ConnectNext::_update_icons() {
	close_button->set_icon(Control::get_theme_icon(SNAME("Close"), EditorStringName(EditorIcons)));

	source_menu->get_popup()->add_icon_radio_check_item(Control::get_theme_icon(SNAME("ExternalLink"), EditorStringName(EditorIcons)), TTR("from built-in")); // SOURCE_BUILT_IN
	source_menu->get_popup()->add_icon_radio_check_item(Control::get_theme_icon(SNAME("Script"), EditorStringName(EditorIcons)), TTR("from script")); // SOURCE_HOST
	source_menu->get_popup()->add_icon_radio_check_item(Control::get_theme_icon(SNAME("Unlinked"), EditorStringName(EditorIcons)), TTR("from link")); // SOURCE_LINK
	refresh_source_menu();

	scope_menu->get_popup()->add_icon_radio_check_item(Control::get_theme_icon(SNAME("CollapseTree"), EditorStringName(EditorIcons)), TTR("base")); // SCOPE_BASE
	scope_menu->get_popup()->add_icon_radio_check_item(Control::get_theme_icon(SNAME("FileTree"), EditorStringName(EditorIcons)), TTR("inheriters")); // SCOPE_INHERITERS
	scope_menu->get_popup()->add_icon_radio_check_item(Control::get_theme_icon(SNAME("ExpandTree"), EditorStringName(EditorIcons)), TTR("unrelated")); // SCOPE_UNRELATED
	refresh_scope_menu();

	search_menu->get_popup()->add_icon_radio_check_item(Control::get_theme_icon(SNAME("Object"), EditorStringName(EditorIcons)), TTR("classes")); // SEARCH_CONSTRUCTORS
	search_menu->get_popup()->add_icon_radio_check_item(Control::get_theme_icon(SNAME("ResourcePreloader"), EditorStringName(EditorIcons)), TTR("constructors")); // SEARCH_CONSTRUCTORS
	search_menu->get_popup()->add_icon_radio_check_item(Control::get_theme_icon(SNAME("MemberMethod"), EditorStringName(EditorIcons)), TTR("methods")); // SEARCH_METHODS
	search_menu->get_popup()->add_icon_radio_check_item(Control::get_theme_icon(SNAME("Add"), EditorStringName(EditorIcons)), TTR("operators")); // SEARCH_OPERATORS
	search_menu->get_popup()->add_icon_radio_check_item(Control::get_theme_icon(SNAME("MemberSignal"), EditorStringName(EditorIcons)), TTR("signals")); // SEARCH_SIGNALS
	search_menu->get_popup()->add_icon_radio_check_item(Control::get_theme_icon(SNAME("MemberConstant"), EditorStringName(EditorIcons)), TTR("constants")); // SEARCH_CONSTANTS
	search_menu->get_popup()->add_icon_radio_check_item(Control::get_theme_icon(SNAME("MemberProperty"), EditorStringName(EditorIcons)), TTR("properties")); // SEARCH_PROPERTIES
	search_menu->get_popup()->add_icon_radio_check_item(Control::get_theme_icon(SNAME("MemberTheme"), EditorStringName(EditorIcons)), TTR("theme items")); // SEARCH_THEME_ITEMS
	refresh_search_menu();
}

void ConnectNext::_draw_debug() {
	if (debug_mouse_inside && mouse_inside) {
		draw_rect(Rect2(Vector2(), get_size()), Color(0.5, 1, 1, 0.3), false, 1.0);
	}
}

void ConnectNext::_update_results(const String &p_search_term) {
	if (search_text->get_text().find("::") != -1) {
		_class_from_search_therm();
		_update_icons();
		results_tree->update_results(search_text->get_text());
	} else {
		results_tree->update_results(p_search_term);
	}
}

void ConnectNext::_class_from_search_therm() {
	PackedStringArray split = search_text->get_text().split("::");
	int carot_col = search_text->get_caret_column();
	int old_text_length = search_text->get_text().length();
	int valid_splits = search_text->get_text().count("::");
	int last_valid_split = -1;

	for (int i = 0; i < valid_splits; i++) {
		if (ClassDB::class_exists(StringName(split[i]))) {
			results_tree->class_hint = split[i];
			results_tree->search_flags = ResultTree::SEARCH_INSIDE_CLASS;
			results_tree->scope_flags = ResultTree::SCOPE_BASE;
			last_valid_split = i;
		}
	}

	String new_search_text = search_text->get_text();
	for (int i = 0; i <= last_valid_split; i++) {
		new_search_text = new_search_text.trim_prefix(split[i] + "::");
	}

	search_text->set_text(new_search_text);
	search_text->set_caret_column(carot_col + new_search_text.length() - old_text_length);
}

void ConnectNext::_tree_confirmed() {
	TreeItem *ti = results_tree->get_selected();

	if (!ti) {
		return;
	}

	if (ti->get_meta("type") == "MethodInfo") {
		_method_info_confirmed(ti->get_meta("MethodInfo"));
	} // else if MethodInfo

	close();
}

void ConnectNext::_method_info_confirmed(Dictionary p_info) {
	MethodInfo mi = MethodInfo::from_dict(p_info);
	Ref<LinkerLink> link = LinkerEditorLayout::create_index_call(mi.name, source_link, argument_links);
	dropped_link->get_host()->add_link(link);
	ERR_PRINT(mi.name);
	// create the new links and refs.
}

void ConnectNext::_move_source_to_argument() {
	if (source_link.is_valid()) {
		argument_links.insert(0, source_link);
		source_link = nullptr;
		_update_link_infos();
	}
}

void ConnectNext::_update_link_infos() {
	if (source_link.is_valid()) {
		source_info = source_link->get_output_info();
	} else {
		source_info = PropertyInfo();
	}
	arguments_info.clear();
	for (int i = 0; i < argument_links.size(); i++) {
		arguments_info.push_back(argument_links[i]->get_output_info());
	}
}

void ConnectNext::dropped(Ref<LinkerLink> p_link, const Point2 &p_point) {
	dropped_link = p_link;
	source_link = p_link;
	_update_link_infos();
	results_tree->class_hint = source_info.class_name;
	search_text->set_text("");
	popup(p_point);
	results_tree->update_results("");
}

void ConnectNext::popup(const Vector2 &p_pos) {
	set_visible(true);
	set_position(p_pos);
	set_size(Vector2(150, 100));
}

void ConnectNext::close() {
	// callable_mp(results_tree, &ResultTree::clear).call_deferred(); // Wait for the Tree's mouse event propagation.
	set_visible(false);
}

ConnectNext::ConnectNext() {
	menu_bar = memnew(HBoxContainer);
	add_child(menu_bar);
	menu_bar->set_alignment(BoxContainer::ALIGNMENT_END);

	source_menu = memnew(MenuButton);
	menu_bar->add_child(source_menu);
	source_menu->get_popup()->set_hide_on_checkable_item_selection(false);
	source_menu->get_popup()->connect("id_pressed", callable_mp(this, &ConnectNext::source_flag_pressed));
	source_menu->get_popup()->connect("popup_hide", callable_mp(this, &ConnectNext::popup_closed)); // update results

	scope_menu = memnew(MenuButton);
	menu_bar->add_child(scope_menu);
	scope_menu->get_popup()->set_hide_on_checkable_item_selection(false);
	scope_menu->get_popup()->connect("id_pressed", callable_mp(this, &ConnectNext::scope_flag_pressed));
	scope_menu->get_popup()->connect("popup_hide", callable_mp(this, &ConnectNext::popup_closed)); // update results

	search_menu = memnew(MenuButton);
	menu_bar->add_child(search_menu);
	search_menu->get_popup()->set_hide_on_checkable_item_selection(false);
	search_menu->get_popup()->connect("id_pressed", callable_mp(this, &ConnectNext::search_flag_pressed));
	search_menu->get_popup()->connect("popup_hide", callable_mp(this, &ConnectNext::popup_closed)); // update results

	close_button = memnew(Button);
	menu_bar->add_child(close_button);
	close_button->set_flat(true);
	close_button->connect("pressed", callable_mp(this, &ConnectNext::close));

	search_text = memnew(LineEdit);
	add_child(search_text);
	search_text->set_tooltip_text(TTR("Enter \" \" to show all filtered options\nEnter \".\" to show all filtered methods, operators and constructors\nUse CTRL_KEY to drop property setters"));
	search_text->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	search_text->connect("text_changed", callable_mp(this, &ConnectNext::_update_results));

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
	results_tree->connect("item_activated", callable_mp(this, &ConnectNext::_tree_confirmed));
	// results_tree->connect("item_activated", callable_mp(this, &EditorHelpSearch::_confirmed));
	// results_tree->connect("item_selected", callable_mp((BaseButton *)get_ok_button(), &BaseButton::set_disabled).bind(false));

	close();
}

ConnectNext::~ConnectNext() {
}
