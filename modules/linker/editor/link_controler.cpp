#include "link_controler.h"

void LinkControler::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			update_node();
			break;
		}
	}
}

void LinkControler::_instantiate() {
	_instantiate_titlebox();
	for (int i = 0; i < link->get_arg_count(); i++) {
		//	Ref<LinkerLink> arg = link->get_arg_link(i);
		LinkRefrence *ref = memnew(LinkRefrence(i, true));
		//	ref->set_refrence(arg);
		argument_refrences->add_child(ref);
	}
	for (int i = 0; i < link->get_output_count(); i++) {
		//	Ref<LinkerLink> arg = link->get_output_link(i);
		LinkRefrence *ref = memnew(LinkRefrence(i, false));
		//	ref->set_refrence(arg);
		output_refrences->add_child(ref);
	}
}

void LinkControler::_instantiate_titlebox() {
	if (link->get_class() == "LinkerIndexGet") {
		//title_icon->set_texture(EditorNode::get_singleton()->get_theme_icon("LinkerIndexGet", "EditorIcons"));
		title_icon->hide();
	} else if (link->get_class() == "LinkerFunction") {
		title_icon->set_texture(Control::get_theme_icon(SNAME("MemberMethod"), EditorStringName(EditorIcons)));
		title->set_text(link->get_caption());
		title->set_tooltip_text(link->get_tooltip());
		return;
	}
	title_box->hide();
	// title_icon->hide();
	// title->hide();
}

void LinkControler::update_node() {
	_instantiate();
	return;

	PropertyInfo pi = link->get_output_info(0);
	Ref<Texture2D> icon = link->get_icon();

	button = memnew(Button);
	add_child(button);
	button->set_flat(true);
	button->set_tooltip_text(String(link->get_tooltip()));
	if (link->get_class() == "LinkerIndexGet") {
		button->set_text(link->get_tooltip());
		button->set_icon_alignment(HORIZONTAL_ALIGNMENT_RIGHT);
	}
	if (link->get_class() == "LinkerFunction") {
		button->set_text(link->get_tooltip());
		button->set_icon_alignment(HORIZONTAL_ALIGNMENT_LEFT);
	}

	if (icon.is_valid()) {
		button->set_icon(icon);
	}

	button->connect("pressed", callable_mp(link.ptr(), &LinkerLink::remove_from_script), CONNECT_DEFERRED);
	button->connect("resized", callable_mp(this, &LinkControler::on_size_changed), CONNECT_DEFERRED);

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

Variant LinkControler::get_drag_data(const Point2 &p_point) {
	ERR_PRINT("LinkControler::get_drag_data: Not implemented");
	return Variant();
}

StringName LinkControler::get_category() const {
	return link->get_category();
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

LinkerLink *LinkControler::get_link() const {
	return link.ptr();
}

LinkControler::LinkControler() {
	set_v_size_flags(SIZE_EXPAND_FILL);
	set_h_size_flags(SIZE_EXPAND_FILL);
	set_focus_mode(Control::FOCUS_ALL);

	VBoxContainer *vbox = memnew(VBoxContainer);
	add_child(vbox);
	title_box = memnew(HBoxContainer);
	vbox->add_child(title_box);
	title = memnew(Label);
	// title->set_mouse_filter(Control::MOUSE_FILTER_IGNORE);
	title_box->add_child(title);
	title_icon = memnew(TextureRect);
	// title_icon->set_mouse_filter(Control::MOUSE_FILTER_IGNORE);
	title_box->add_child(title_icon);

	HBoxContainer *hbox = memnew(HBoxContainer);
	vbox->add_child(hbox);
	// hbox->set_mouse_filter(Control::MOUSE_FILTER_IGNORE);
	argument_refrences = memnew(VBoxContainer);
	hbox->add_child(argument_refrences);
	output_refrences = memnew(VBoxContainer);
	hbox->add_child(output_refrences);
}
