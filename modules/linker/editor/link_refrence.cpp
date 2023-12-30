#include "link_refrence.h"

void LinkRefrence::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_POST_ENTER_TREE: {
			Control *parent = get_parent_control();
			while (parent != nullptr) {
				if (parent->get_class() == "LinkControler") {
					LinkControler *controler = Object::cast_to<LinkControler>(parent);
					host = Ref<LinkerLink>(controler->get_link());
					return;
				}
				parent = parent->get_parent_control();
			}
			ERR_PRINT("LinkRefrence: is not child of LinkControler!");
			break;
		}
		case NOTIFICATION_READY: {
			if (host.is_valid()) {
				if (is_input) {
					int max_idx = host->get_arg_count() - 1;
					if (link_idx >= 0 && link_idx <= max_idx) {
						info = host->get_arg_info(link_idx);
						_instantiate();
						return;
					}
				} else {
					int max_idx = host->get_output_count() - 1;
					if (link_idx >= 0 && link_idx <= max_idx) {
						info = host->get_output_info(link_idx);
						_instantiate();
						return;
					}
				}
			}
			ERR_PRINT("LinkRefrence is not valid");
			break;
		}
	}
}

void LinkRefrence::_instantiate() {
	if (is_input) {
		add_theme_constant_override("margin_left", 20);
		add_theme_constant_override("margin_top", 1);
		add_theme_constant_override("margin_right", 1);
		add_theme_constant_override("margin_bottom", 1);
	} else {
		add_theme_constant_override("margin_left", 1);
		add_theme_constant_override("margin_top", 1);
		add_theme_constant_override("margin_right", 20);
		add_theme_constant_override("margin_bottom", 1);
	}

	Button *button = memnew(Button);
	add_child(button);
	button->set_flat(true);
	button->set_tooltip_text(String(info.name));
	button->set_text(String(info.name));
	if (info.type == Variant::OBJECT) {
		button->set_icon(EditorNode::get_singleton()->get_class_icon(info.class_name));
	} else {
		button->set_icon(EditorNode::get_singleton()->get_class_icon(Variant::get_type_name(info.type)));
	}
	if (is_input) {
		button->set_icon_alignment(HORIZONTAL_ALIGNMENT_LEFT);
	} else {
		button->set_icon_alignment(HORIZONTAL_ALIGNMENT_RIGHT);
	}
	if (label_override != "") {
		button->set_text(label_override);
	}
}

Variant LinkRefrence::get_drag_data(const Point2 &p_point) {
	ERR_PRINT("LinkRefrence::get_drag_data: Not implemented");
	return Variant();
}

void LinkRefrence::override_label(String p_label) {
	label_override = p_label;
	if (get_child_count() > 0) {
		Button *button = Object::cast_to<Button>(get_child(0));
		if (button) {
			button->set_text(p_label);
		}
	}
}

LinkRefrence::LinkRefrence(int p_idx, bool p_is_input) {
	link_idx = p_idx;
	is_input = p_is_input;
}