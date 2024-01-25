#include "link_controler.h"
#include "link_connection.h"
#include "linker_editor_layout.h"

void LinkControler::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			_instantiate();
			queue_redraw();
			break;
		}
		case NOTIFICATION_DRAG_BEGIN: {
			dragging = mouse_inside;
			queue_redraw();
			break;
		}
		case NOTIFICATION_DRAG_END: {
			dragging = false;
			queue_redraw();
			break;
		}
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
		margin_left = 20;
		margin_right = 1;
		margin_top = 1;
		margin_bottom = 1;
	}
	if (p_align == HORIZONTAL_ALIGNMENT_RIGHT) {
		margin_left = 1;
		margin_right = 20;
		margin_top = 1;
		margin_bottom = 1;
	}
	add_theme_constant_override(SNAME("margin_left"), margin_left);
	add_theme_constant_override(SNAME("margin_top"), margin_top);
	add_theme_constant_override(SNAME("margin_right"), margin_right);
	add_theme_constant_override(SNAME("margin_bottom"), margin_bottom);
}

void LinkControler::_draw_debug() {
	if (debug_drag_and_drop && (mouse_inside || dragging)) {
		draw_rect(Rect2(Vector2(0.0, 0.0), get_size()), Color(0.5, 1, 1, 0.3), false, 1.0);
	}
	if (debug_node_id) {
		Ref<Font> font = get_theme_font(SNAME("font"), SNAME("Label"));
		int font_size = get_theme_font_size(SNAME("font_size"), SNAME("Label"));
		draw_string(font, get_size() / 2, itos(link->get_link_idx()), HORIZONTAL_ALIGNMENT_CENTER, -1, font_size, Color(0, 0, 0, 1));
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

Vector2 LinkControler::get_connection_point_top() const {
	Vector2 pos = get_position();
	Vector2 size = get_size();
	int center = pos.x + margin_left + (size.x - margin_left - margin_right) / 2;
	int top = pos.y + margin_top;
	return Vector2(center, top);
}

Vector2 LinkControler::get_connection_point_bottom() const {
	Vector2 pos = get_position();
	Vector2 size = get_size();
	int center = pos.x + margin_left + (size.x - margin_left - margin_right) / 2;
	int bottom = pos.y + size.y - margin_bottom;
	return Vector2(center, bottom);
}

Vector2 LinkControler::get_connection_point_left() const {
	Vector2 pos = get_position();
	Vector2 size = get_size();
	int left = pos.x + margin_left;
	int center = pos.y + margin_top + (size.y - margin_top - margin_bottom) / 2;
	return Vector2(left, center);
}

Vector2 LinkControler::get_connection_point_right() const {
	Vector2 pos = get_position();
	Vector2 size = get_size();
	int right = pos.x + size.x - margin_right;
	int center = pos.y + margin_top + (size.y - margin_top - margin_bottom) / 2;
	return Vector2(right, center);
}

LinkControler::LinkControler() {
	set_v_size_flags(SIZE_EXPAND_FILL);
	set_h_size_flags(SIZE_EXPAND_FILL);
	set_focus_mode(Control::FOCUS_ALL);
}

LinkControler::~LinkControler() {
}
