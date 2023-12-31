#include "link_refrence.h"

void LinkRefrence::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_POST_ENTER_TREE: {
			Control *parent = get_parent_control();
			while (parent != nullptr) {
				if (parent->get_class() == "LinkControler") {
					LinkControler *controler = Object::cast_to<LinkControler>(parent);
					host = Ref<LinkerLink>(controler->get_link());
					break;
				}
				parent = parent->get_parent_control();
			}
			ERR_PRINT("LinkRefrence::_notification: " + String(get_parent()->get_class()) + " is not LinkControler");
			break;
		}
		case NOTIFICATION_READY: {
			if (host.is_valid()) {
				if (is_input) {
					int max_idx = host->get_arg_count() - 1;
					if (link_idx >= 0 && link_idx <= max_idx) {
						info = host->get_arg_info(link_idx);
						_instantiate();
						break;
					}
				} else {
					int max_idx = host->get_output_count() - 1;
					if (link_idx >= 0 && link_idx <= max_idx) {
						info = host->get_output_info(link_idx);
						_instantiate();
						break;
					}
				}
			}
			ERR_PRINT("LinkRefrence is not valid");
			break;
		}
	}
}

void LinkRefrence::_instantiate() {
	Button *button = memnew(Button);
	add_child(button);
	button->set_flat(true);
	button->set_mouse_filter(Control::MOUSE_FILTER_IGNORE);
	button->set_tooltip_text(String(info.name));
	button->set_text(String(info.name));
	if (info.type == Variant::OBJECT) {
		button->set_icon(EditorNode::get_singleton()->get_class_icon(info.class_name));
	} else {
		button->set_icon(EditorNode::get_singleton()->get_class_icon(Variant::get_type_name(info.type)));
	}
	if (is_input) {
		button->set_icon_alignment(HORIZONTAL_ALIGNMENT_RIGHT);
	} else {
		button->set_icon_alignment(HORIZONTAL_ALIGNMENT_LEFT);
	}
}

Variant LinkRefrence::get_drag_data(const Point2 &p_point) {
	return Variant();
}

LinkRefrence::LinkRefrence(int p_idx, bool p_is_input) {
	link_idx = p_idx;
	is_input = p_is_input;
}
