#include "link_controler.h"
#include "link_connection.h"
#include "linker_editor_layout.h"

Rect2 LinkControler::rect_this() const {
	if (!is_visible()) {
		return Rect2();
	}
	Rect2 r = get_rect();
	r.position = Vector2(0, 0);
	return r;
}

Rect2 LinkControler::rect_source() const {
	if (!source_rect->is_visible()) {
		return Rect2();
	}
	Rect2 r;
	r.size.y = banner->get_size().y;
	r.size.x = label->get_position().x;
	return r;
}

Rect2 LinkControler::rect_arg() const {
	if (!arguments->is_visible()) {
		return Rect2();
	}
	Rect2 r = arguments->get_rect();
	r.position += components->get_position();
	return r;
}

Rect2 LinkControler::rect_value_1() const {
	if (!value_rect_1->is_visible()) {
		return Rect2();
	}
	Rect2 r = value_rect_1->get_rect();
	r.position += components->get_position();
	return r;
}

Rect2 LinkControler::rect_index() const {
	if (!label->is_visible()) {
		return Rect2();
	}
	Rect2 r = label->get_rect();
	return r;
}

Rect2 LinkControler::rect_icon() const {
	if (!icon->is_visible()) {
		return Rect2();
	}
	Rect2 r;
	r.position.x = label->get_position().x + label->get_size().x;
	r.size.y = banner->get_size().y;
	r.size.x = banner->get_size().x - r.position.x;
	return r;
}

Rect2 LinkControler::rect_push() const {
	if (!push_rect->is_visible()) {
		return Rect2();
	}
	Rect2 r = push_rect->get_rect();
	r.position += components->get_position();
	return r;
}

Rect2 LinkControler::rect_value_2() const {
	if (!value_rect_2->is_visible()) {
		return Rect2();
	}
	Rect2 r = value_rect_2->get_rect();
	return r;
}

void LinkControler::resize_font_of(Label *p_label, int p_font_size) {
	Ref<LabelSettings> settings;
	settings.instantiate();
	settings->set_font_size(p_font_size);
	p_label->set_label_settings(settings);
}

void LinkControler::_bind_methods() {
	ADD_SIGNAL(MethodInfo("starting_edit_mode", PropertyInfo(Variant::OBJECT, "controler")));
}

void LinkControler::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			_instantiate();
			queue_redraw();
			break;
		}
		case NOTIFICATION_PROCESS: {
			if (!dragging && !mouse_inside) {
				set_process(false);
			}
			if (!dragging && !lmb_down) { // do not change layout during dragging
				if (mouse_inside && !edit_mode) {
					if (OS::get_singleton()->get_ticks_msec() - last_mouse_enter > 750) {
						start_edit_mode();
					}
				}
				if (edit_mode && !mouse_inside) {
					end_edit_mode();
				}
				if (mouse_inside && edit_mode) {
					update_edit_mode();
				}
			}
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
			lmb_down = false;
			drag_data = Variant();
			queue_redraw();
			break;
		}
		case NOTIFICATION_MOUSE_ENTER: {
			last_mouse_enter = OS::get_singleton()->get_ticks_msec();
			set_process(true);
			mouse_inside = true;
			dragging_from = Input::get_singleton()->is_mouse_button_pressed(MouseButton::LEFT);
			queue_redraw();
			break;
		}
		case NOTIFICATION_MOUSE_EXIT: {
			mouse_inside = false;
			dragging_from = false;
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
	arguments->set_h_size_flags(SIZE_SHRINK_BEGIN);
	arguments->set_v_size_flags(SIZE_SHRINK_CENTER);
	add_theme_constant_override("separation", 0);
	banner->add_theme_constant_override("separation", 0);
	components->add_theme_constant_override("separation", 0);
	arguments->add_theme_constant_override("separation", 0);
	value_rect_1->set_h_size_flags(SIZE_EXPAND_FILL);

	label->set_text(_get_text(link.ptr()));
	label->set_tooltip_text(String(link->get_class()));
	edit_index->hide();
	icon->set_texture(_get_link_icon(link.ptr()));
	icon->set_stretch_mode(TextureRect::STRETCH_KEEP);
	icon->set_v_size_flags(SIZE_SHRINK_CENTER);
	icon->set_h_size_flags(SIZE_SHRINK_CENTER);

	for (int i = 0; i < link->get_argument_count(); i++) {
		PropertyInfo pi = link->get_input_value_port_info(i);
		Label *_label = memnew(Label);
		_label->set_text(pi.name);
		arguments->add_child(_label);
		resize_font_of(_label, 10);
	}

	edit_index->connect("text_submitted", callable_mp(link.ptr(), &LinkerLink::set_index), CONNECT_DEFERRED);

	set_component_visibility();
}

void LinkControler::set_component_visibility() {
	push_rect->set_custom_minimum_size(CELL_SIZE_X);
	value_rect_1->set_custom_minimum_size(CELL_SIZE_SMALL_Y);

	if (link->use_source()) {
		source_rect->set_custom_minimum_size(CELL_SIZE_SMALL);
	} else {
		source_rect->set_custom_minimum_size(CELL_SIZE_HIDDEN);
	}
	push_rect->set_custom_minimum_size(CELL_SIZE_SMALL);
}

void LinkControler::on_size_changed() {
	// if (button) {
	// 	set_custom_minimum_size(Vector2(button->get_size().x, button->get_size().y));
	// }
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

void LinkControler::_draw_debug() {
	if (debug_drag_and_drop && (mouse_inside || dragging)) {
		draw_rect(Rect2(Vector2(0.0, 0.0), get_size()), Color(0.5, 1, 1, 0.3), false, 1.0);
	}
	String debug_string = "";

	if (debug_node_id) {
		debug_string += itos(link->get_link_idx()) + " ";
		// Ref<Font> font = get_theme_font(SNAME("font"), SNAME("Label"));
		// int font_size = get_theme_font_size(SNAME("font_size"), SNAME("Label"));
		// draw_string(font, get_size() / 2, itos(link->get_link_idx()), HORIZONTAL_ALIGNMENT_CENTER, -1, font_size, Color(0, 0, 0, 1));
	}
	if (debug_ref_id) {
		debug_string += itos(link->get_instance_id()) + " ";
	}
	if (debug_controler_id) {
		debug_string += to_string() + " ";
	}
	if (debug_string != "") {
		Ref<Font> font = get_theme_font(SNAME("font"), SNAME("Label"));
		int font_size = get_theme_font_size(SNAME("font_size"), SNAME("Label"));
		draw_string(font, get_size() / 2, debug_string, HORIZONTAL_ALIGNMENT_CENTER, -1, font_size, Color(0, 0, 0, 1));
	}

	if (dragging_from || edit_mode) {
		Point2 mouse_pos = get_local_mouse_position();
		draw_rect(rect_this(), Color(0.1, 0.9, 0.9, 0.3), false); //rect_this().has_point(mouse_pos));
		draw_rect(rect_source(), Color(0.9, 0.1, 0.1, 0.3), rect_source().has_point(mouse_pos));
		draw_rect(rect_arg(), Color(0.1, 0.9, 0.1, 0.3), rect_arg().has_point(mouse_pos));
		draw_rect(rect_value_1(), Color(0.1, 0.1, 0.9, 0.3), rect_value_1().has_point(mouse_pos));
		draw_rect(rect_index(), Color(0.1, 0.9, 0.1, 0.3), rect_index().has_point(mouse_pos));
		draw_rect(rect_icon(), Color(0.1, 0.1, 0.9, 0.3), rect_icon().has_point(mouse_pos));
		draw_rect(rect_push(), Color(0.9, 0.1, 0.1, 0.3), rect_push().has_point(mouse_pos));
		draw_rect(rect_value_2(), Color(0.1, 0.9, 0.1, 0.3), rect_value_2().has_point(mouse_pos));
	}
}

void LinkControler::gui_input(const Ref<InputEvent> &p_event) {
	if (p_event->is_action("ui_graph_delete")) {
		link->remove_from_script();
		get_viewport()->set_input_as_handled();
	}
	if (p_event->is_action("ui_select")) {
		// default press spacebar
		//	button->grab_focus();
		get_viewport()->set_input_as_handled();
	}
	Ref<InputEventMouseButton> mb = p_event;
	if (mb.is_valid()) {
		if (mb->is_double_click()) {
			// popup menu
			ERR_PRINT("create popup");
			//		button->grab_focus();
			get_viewport()->set_input_as_handled();
		}
		if (!mb->get_button_mask().has_flag(MouseButtonMask::LEFT)) {
			lmb_down = false;
		} else {
			if (!lmb_down) {
				prep_drag_data();
			}
			lmb_down = true;
		}
	}
}

void LinkControler::prep_drag_data() {
	// prep on mouse down as start drag data has no acurate mouseposition and the layout may be changed as well.
	Point2 mouse_pos = get_local_mouse_position();
	if (rect_source().has_point(mouse_pos)) {
		drag_data = link->get_source()->get_drag_data();
	} else if (rect_arg().has_point(mouse_pos)) {
		Vector2 arg_pos = mouse_pos - rect_arg().position;
		int arg_count = link->get_argument_count();
		drag_data = link->get_drag_data();
		drag_data["link_request"] = "get_arg";
		drag_data["link_arg_index"] = arg_pos.y / (rect_arg().size.y / arg_count);
	} else if (rect_value_1().has_point(mouse_pos)) {
		drag_data = link->get_drag_data();
		drag_data["link_request"] = "use_part_of_value";
	} else if (rect_index().has_point(mouse_pos)) {
		drag_data = link->get_drag_data();
		drag_data["link_request"] = "use_output";
	} else if (rect_icon().has_point(mouse_pos)) {
		drag_data = link->get_drag_data();
		drag_data["link_request"] = "use_output";
	} else if (rect_push().has_point(mouse_pos)) {
		drag_data = link->get_drag_data();
		drag_data["link_request"] = "push_next_command";
	} else if (rect_value_2().has_point(mouse_pos)) {
		drag_data = link->get_drag_data();
		drag_data["link_request"] = "use_part_of_value";
	} else if (rect_this().has_point(mouse_pos)) {
		drag_data = link->get_drag_data();
		drag_data["link_request"] = "use_output";
	} else {
		ERR_PRINT("no drag data");
	}
}

Variant LinkControler::get_drag_data(const Point2 &p_point) {
	return drag_data;
}

bool LinkControler::can_drop_data(const Point2 &p_point, const Variant &p_data) const {
	Dictionary d_data = p_data;
	if (!d_data.has("type") ||
			d_data["type"] != "linker_link") {
		return false;
	}
	Ref<LinkerLink> dropped_link = link->get_host()->get_link(d_data["link_idx"]);
	if (dropped_link.is_null() || dropped_link->get_host() != link->get_host()) {
		// drag link is not vallid or not part of the same script
		return false;
	}
	Point2 mouse_pos = get_local_mouse_position();
	if (rect_source().has_point(mouse_pos)) {
		// use the drop link as a new source
		if (d_data.has("link_request")) {
			return (
					(d_data["link_request"] == "use_output") ||
					(d_data["link_request"] == "push_next_command"));
		}
	} else if (rect_arg().has_point(mouse_pos)) {
		if (d_data.has("link_request")) {
			return (
					(d_data["link_request"] == "use_output") ||
					(d_data["link_request"] == "push_next_command"));
		}
	} else if (rect_value_1().has_point(mouse_pos)) {
		if (d_data.has("link_request")) {
			return (
					(d_data["link_request"] == "use_output") ||
					(d_data["link_request"] == "push_next_command"));
		}
	} else if (rect_push().has_point(mouse_pos)) {
		if (d_data.has("link_request")) {
			return (
					(d_data["link_request"] == "use_output") ||
					(d_data["link_request"] == "push_next_command"));
		}
	} else if (rect_value_2().has_point(mouse_pos)) {
		return false;
	} else if (rect_this().has_point(mouse_pos)) {
		if (d_data.has("link_request")) {
			return (
					(d_data["link_request"] == "get_arg") ||
					(d_data["link_request"] == "get_part_of_value"));
		}
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

	Point2 mouse_pos = get_local_mouse_position();
	if (rect_source().has_point(mouse_pos)) {
		if (d_data.has("link_request")) {
			if (d_data["link_request"] == "use_output") {
				link->set_source(drag_link);
			} else if (d_data["link_request"] == "get_arg") {
				ERR_PRINT("not able to set arguments");
			} else if (d_data["link_request"] == "push_next_command") {
				drag_link->add_push_link_ref(link);
			} else if (d_data["link_request"] == "get_part_of_value") {
				ERR_PRINT("get_part_of_value not implemented jet");
			}
		}
	} else if (rect_arg().has_point(mouse_pos)) {
		if (d_data.has("link_request")) {
			Vector2 arg_pos = mouse_pos - rect_arg().position;
			int arg_count = link->get_argument_count();
			int arg_idx = arg_pos.y / (rect_arg().size.y / arg_count);
			if (d_data["link_request"] == "use_output") {
				link->add_arg_link_ref(drag_link, arg_idx);
			} else if (d_data["link_request"] == "get_arg") {
				ERR_PRINT("not able to get argument from argument");
			} else if (d_data["link_request"] == "push_next_command") {
				ERR_PRINT("not able to push setting of an argument");
			} else if (d_data["link_request"] == "get_part_of_value") {
				ERR_PRINT("get_part_of_value not implemented jet");
			}
		}
	} else if (rect_value_1().has_point(mouse_pos)) {
		if (d_data.has("link_request")) {
			if (d_data["link_request"] == "use_output") {
				ERR_PRINT("not able to set memeber of a value");
			} else if (d_data["link_request"] == "get_arg") {
				ERR_PRINT("not able to set memeber of a value");
			} else if (d_data["link_request"] == "push_next_command") {
				ERR_PRINT("not able to push memeber of a value");
			} else if (d_data["link_request"] == "get_part_of_value") {
				ERR_PRINT("not able to set memeber of a value");
			}
		}
	} else if (rect_push().has_point(mouse_pos)) {
		link->add_push_link_ref(drag_link);
		// if (d_data["link_request"] == "use_output") {
		// 	link->add_push_link_ref(drag_link);
		// } else if (d_data["link_request"] == "get_arg") {
		// 	ERR_PRINT("not able to push arguments");
		// } else if (d_data["link_request"] == "push_next_command") {
		// 	drag_link->add_push_link_ref(link);
		// } else if (d_data["link_request"] == "get_part_of_value") {
		// 	ERR_PRINT("not able to set memeber of a value");
		// }
	} else if (rect_value_2().has_point(mouse_pos)) {
		// return link->can_drop_on_link(drag_link);
	} else if (rect_this().has_point(mouse_pos)) {
		if (d_data.has("link_request")) {
			if (d_data["link_request"] == "use_output") {
				link->set_source(drag_link);
			} else if (d_data["link_request"] == "get_arg") {
				drag_link->add_arg_link_ref(link, d_data["link_arg_index"]);
			} else if (d_data["link_request"] == "push_next_command") {
				drag_link->add_push_link_ref(link);
			} else if (d_data["link_request"] == "get_part_of_value") {
				ERR_PRINT("not able to set memeber of a value");
			}
		}
	}
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
	int center = pos.x + (size.x / 2);
	int top = pos.y;
	return Vector2(center, top);
}

Vector2 LinkControler::get_connection_point_bottom() const {
	Vector2 pos = get_position();
	Vector2 size = get_size();
	int center = pos.x + (size.x / 2);
	int bottom = pos.y + size.y;
	return Vector2(center, bottom);
}

Vector2 LinkControler::get_connection_point_left() const {
	Vector2 pos = get_position();
	Vector2 size = get_size();
	int center = pos.y + (size.y / 2);
	int left = pos.x;
	return Vector2(left, center);
}

Vector2 LinkControler::get_connection_point_right() const {
	Vector2 pos = get_position();
	Vector2 size = get_size();
	int center = pos.y + (size.y / 2);
	int right = pos.x + size.x;
	return Vector2(right, center);
}

void LinkControler::start_edit_mode() {
	edit_mode = true;
	//	emit_signal("starting_edit_mode", this);

	edit_mode_line_edit = !dragging_from;

	if (edit_mode_line_edit) {
		// edit_index->set_text(String(link->get_index()));
		// edit_index->show();
		// label->hide();
	}
	if (dragging_from) {
		// expand i/o rects
	}

	// add background panel
	// add argument draggables
	// add inspector
	// add drop sequence box
	// add select sequence box to access edit methods
	// edit sequence order
	set_size(Vector2(0, 0));
}

void LinkControler::update_edit_mode() {
	set_component_visibility();
	if (value_rect_1->get_size().x < CELL_SIZE.x * 2) {
		value_rect_2->set_custom_minimum_size(CELL_SIZE_SMALL_Y);
	} else {
		value_rect_2->set_custom_minimum_size(CELL_SIZE_HIDDEN);
	}
	Point2 mouse_pos = get_local_mouse_position();
	if (rect_source().has_point(mouse_pos)) {
		source_rect->set_custom_minimum_size(CELL_SIZE);
	} else if (rect_push().has_point(mouse_pos)) {
		push_rect->set_custom_minimum_size(CELL_SIZE);
	}
	set_size(Vector2(0, 0));
}

void LinkControler::end_edit_mode() {
	edit_index->hide();
	label->show();
	edit_mode = false;
	set_component_visibility();
	set_size(Vector2(0, 0));
}

LinkControler::LinkControler() {
	set_mouse_filter(Control::MOUSE_FILTER_STOP);
	set_focus_mode(Control::FOCUS_ALL);
	banner = memnew(HBoxContainer);
	add_child(banner);
	components = memnew(HBoxContainer);
	add_child(components);

	source_rect = memnew(Control);
	banner->add_child(source_rect);
	label = memnew(Label);
	banner->add_child(label);
	edit_index = memnew(LineEdit);
	banner->add_child(edit_index);
	icon = memnew(TextureRect);
	banner->add_child(icon);

	arguments = memnew(VBoxContainer);
	components->add_child(arguments);
	value_rect_1 = memnew(Control);
	components->add_child(value_rect_1);
	push_rect = memnew(Control);
	components->add_child(push_rect);

	value_rect_2 = memnew(Control);
	add_child(value_rect_2);

	label->set_mouse_filter(Control::MOUSE_FILTER_PASS);
	// edit_index->set_mouse_filter(Control::MOUSE_FILTER_PASS);
	icon->set_mouse_filter(Control::MOUSE_FILTER_PASS);
	source_rect->set_mouse_filter(Control::MOUSE_FILTER_PASS);
	arguments->set_mouse_filter(Control::MOUSE_FILTER_PASS);
	value_rect_1->set_mouse_filter(Control::MOUSE_FILTER_PASS);
	push_rect->set_mouse_filter(Control::MOUSE_FILTER_PASS);
	value_rect_2->set_mouse_filter(Control::MOUSE_FILTER_PASS);

	source_rect->set_tooltip_text("source");
	arguments->set_tooltip_text("argument");
	value_rect_1->set_tooltip_text("set_memeber");
	push_rect->set_tooltip_text("drag to push new commands");
	value_rect_2->set_tooltip_text("only show when value rect 1 is to small");
	set_tooltip_text("link_controler");
}

LinkControler::~LinkControler() {
}