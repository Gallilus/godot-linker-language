#include "link_controler.h"

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

	button->connect("pressed", callable_mp(link.ptr(), &LinkerLink::remove_from_script).bind(false), CONNECT_DEFERRED);
	button->connect("resized", callable_mp(this, &LinkControler::on_size_changed), CONNECT_DEFERRED);
}

void LinkControler::update_connection() {
	// update connections
	// link->get_arg_links()
	//      add connection_point and connect to destination as routed by GraphTheory
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
