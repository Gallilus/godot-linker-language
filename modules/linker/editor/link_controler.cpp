#include "link_controler.h"
#include "link_connection.h"
#include "linker_editor_layout.h"

void LinkControler::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			_instantiate();
			break;
		}
	}
}

void LinkControler::_instantiate() {
	if (!link.is_valid()) {
		return;
	}
	PropertyInfo pi = link->get_output_info();
	button = memnew(Button);
	add_child(button);
	button->set_flat(true);
	button->set_tooltip_text(String(link->get_class()));
	button->set_text(_get_text(link.ptr()));
	button->set_icon(_get_link_icon(link.ptr()));
	button->set_icon_alignment(_get_icon_h_alignement(link.ptr()));

	//button->connect("pressed", callable_mp(link.ptr(), &LinkerLink::remove_from_script).bind(false), CONNECT_DEFERRED);
	button->set_drag_forwarding(callable_mp(this, &LinkControler::get_drag_data), callable_mp(this, &LinkControler::can_drop_data), callable_mp(this, &LinkControler::drop_data));
	button->connect("resized", callable_mp(this, &LinkControler::on_size_changed), CONNECT_DEFERRED);
	button->set_mouse_filter(Control::MOUSE_FILTER_PASS);

	_set_margin(_get_icon_h_alignement(link.ptr()));
}

void LinkControler::update_connection() {
	if (link->has_source()) {
		connection = memnew(LinkConnection);
		get_parent()->add_child(connection);
		connection->set_start(layout->get_link_source_controler(link));
		connection->set_end(this);
	}
}

void LinkControler::on_size_changed() {
	if (button) {
		set_custom_minimum_size(Vector2(button->get_size().x, button->get_size().y));
	}
}

Ref<Texture2D> LinkControler::_get_link_icon(LinkerLink *p_link) const {
	PropertyInfo pi = p_link->get_output_info();

	if (pi.type == Variant::OBJECT) {
		return EditorNode::get_singleton()->get_class_icon(pi.class_name);
	} else {
		return EditorNode::get_singleton()->get_class_icon(Variant::get_type_name(pi.type));
	}

	if (p_link->get_class() == "LinkerFunction") {
		return Control::get_theme_icon(SNAME("MemberMethod"), EditorStringName(EditorIcons));
	}

	return Ref<Texture2D>();
}

HorizontalAlignment LinkControler::_get_icon_h_alignement(LinkerLink *p_link) const {
	if (p_link->get_graph_category() == "graph_input") {
		return HORIZONTAL_ALIGNMENT_RIGHT;
	}
	return HORIZONTAL_ALIGNMENT_LEFT;
}

String LinkControler::_get_text(LinkerLink *p_link) const {
	if (p_link->get_class() == "LinkerSceneRefrence") {
		LinkerSceneRefrence *scene_ref = Object::cast_to<LinkerSceneRefrence>(p_link);
		if (scene_ref) {
			return String(scene_ref->get_node_scene_relative_path());
		}
	}
	return String(p_link->get_index());
}

void LinkControler::_set_margin(HorizontalAlignment p_align) {
	if (p_align == HORIZONTAL_ALIGNMENT_LEFT) {
		add_theme_constant_override(SNAME("margin_left"), 20);
		add_theme_constant_override(SNAME("margin_right"), 1);
		add_theme_constant_override(SNAME("margin_top"), 1);
		add_theme_constant_override(SNAME("margin_bottom"), 1);
	}
	if (p_align == HORIZONTAL_ALIGNMENT_RIGHT) {
		add_theme_constant_override(SNAME("margin_left"), 1);
		add_theme_constant_override(SNAME("margin_right"), 20);
		add_theme_constant_override(SNAME("margin_top"), 1);
		add_theme_constant_override(SNAME("margin_bottom"), 1);
	}
}

void LinkControler::gui_input(const Ref<InputEvent> &p_event) {
	if (p_event->is_action("ui_graph_delete")) {
		link->remove_from_script();
		get_viewport()->set_input_as_handled();
	}
	if (p_event->is_action("ui_select")) {
		// default press spacebar
		button->grab_focus();
		get_viewport()->set_input_as_handled();
	}
	Ref<InputEventMouseButton> mb = p_event;
	if (mb.is_valid()) {
		if (mb->is_double_click()) {
			// popup menu
			ERR_PRINT("create popup");
			button->grab_focus();
			get_viewport()->set_input_as_handled();
		}
	}
}

Variant LinkControler::get_drag_data(const Point2 &p_point) {
	return link->get_drag_data();
}

bool LinkControler::can_drop_data(const Point2 &p_point, const Variant &p_data) const {
	Dictionary d_data = p_data;
	if (!d_data.has("type") ||
			d_data["type"] != "linker_link") {
		return false;
	}
	Ref<LinkerLink> drag_link = link->get_host()->get_link(d_data["link_idx"]);
	if (drag_link.is_null() ||
			drag_link->get_host() != link->get_host()) {
		return false;
	}
	if (link->can_drop(drag_link)) {
		return true;
	}
	return false;
}

void LinkControler::drop_data(const Point2 &p_point, const Variant &p_data) {
	Dictionary d_data = p_data;
	if (!d_data.has("type") ||
			d_data["type"] != "linker_link") {
		return;
	}
	Ref<LinkerLink> drag_link = link->get_host()->get_link(d_data["link_idx"]);
	link->drop_data(drag_link);
}

void LinkControler::set_link(Ref<LinkerLink> p_link) {
	link = p_link;
	link->connect("removed_from_script", Callable(this, "queue_free"));
	// connect signals, selected, etc.
	// dragging = gostline
	// ctrl + drag = copy
	// alt + drag = move
	// select = Graph_inspector to position to inspect
	// ctrl + select = add to graph_search_focus
}

LinkControler::LinkControler() {
	set_v_size_flags(SIZE_EXPAND_FILL);
	set_h_size_flags(SIZE_EXPAND_FILL);
	set_focus_mode(Control::FOCUS_ALL);
}

LinkControler::~LinkControler() {
	if (connection) {
		connection->queue_free();
	}
}
