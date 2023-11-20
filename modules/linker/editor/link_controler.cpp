#include "link_controler.h"

void LinkControler::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			update_node();
			break;
		}
	}
}

void LinkControler::update_node() {
	PropertyInfo pi = link->get_output_info();
	Ref<Texture2D> icon = EditorNode::get_singleton()->get_class_icon(pi.class_name, "Variant");

	button = memnew(Button);
	add_child(button);
	button->set_tooltip_text(String(link->get_tooltip()));
	button->set_flat(true);
	if (icon.is_valid()) {
		button->set_icon(icon);
	}

	button->connect("pressed", callable_mp(link.ptr(), &LinkerLink::remove_from_script), CONNECT_DEFERRED);
	button->connect("resized", callable_mp(this, &LinkControler::on_size_changed), CONNECT_DEFERRED);
	// connect on size changed to edit the size of LinkControl

	// ports
	//      link->get_arg_links().size
	//          inbedding Links or creating boxes is to be avoided at all times
	//          Reserve space for ports
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

void LinkControler::set_link(Ref<LinkerLink> p_link) {
	link = p_link;
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
