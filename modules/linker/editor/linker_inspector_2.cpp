#include "linker_inspector_2.h"

void LinkerInspector2::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_editing_value"), &LinkerInspector2::get_editing_value);
	ClassDB::bind_method(D_METHOD("set_editing_value", "value"), &LinkerInspector2::set_editing_value);

	ADD_PROPERTY(PropertyInfo(Variant::NIL, "editing_value"), "set_editing_value", "get_editing_value");
}

void LinkerInspector2::setup_property_editor(EditorProperty *p_editor_property) {
	if (!p_editor_property) {
		return;
	}
	p_editor_property->set_focus_mode(FOCUS_ALL);
	p_editor_property->set_object_and_property(this, StringName("editing_value"));
	p_editor_property->update_property();
	p_editor_property->set_custom_minimum_size(Vector2(100, 100));
	p_editor_property->set_name_split_ratio(0.5); // hide name label
	p_editor_property->connect("focus_exited", callable_mp((Node *)p_editor_property, &Node::queue_free)); // if not selected, remove
	//p_editor_property->connect("property_changed", callable_mp(this, &LinkerInspector::_property_changed).bind(false));
}

void LinkerInspector2::set_object(Ref<LinkerLink> p_link) {
	link = p_link;
	editing_value = link->get_placeholder_value();
	EditorProperty *editor = nullptr;

	PropertyInfo pi = PropertyInfo::from_dict(link->get_placeholder_info()["value"]);

	Node *node = Object::cast_to<Node>(editing_value);
	if (node) {
		ERR_PRINT("can not edit Node's need to create a select from tree for this");
		return;
	}

	editor = EditorInspector::instantiate_property_editor(this, pi.type, "editing_value", pi.hint, pi.hint_string, pi.usage);
	if (editor) {
		setup_property_editor(editor);
		editor->set_v_size_flags(SIZE_EXPAND_FILL);
		editor->set_h_size_flags(SIZE_EXPAND_FILL);
		add_child(editor);
		hide();
	}
}

void LinkerInspector2::popup(Point2 p_pos) {
	set_position(p_pos);
	if (get_child_count() > 0) {
		get_child(0)->call_deferred("grab_focus");
	}
	show();
}
