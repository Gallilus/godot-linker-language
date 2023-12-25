#include "linker_inspector.h"
#include "editor/editor_string_names.h"

void LinkerInspector::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_editing_value"), &LinkerInspector::get_editing_value);
	ClassDB::bind_method(D_METHOD("set_editing_value", "value"), &LinkerInspector::set_editing_value);

	ADD_PROPERTY(PropertyInfo(Variant::NIL, "editing_value"), "set_editing_value", "get_editing_value");
}

void LinkerInspector::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
		} break;
		case NOTIFICATION_READY: {
			ERR_FAIL_NULL(script);
			ERR_FAIL_COND_MSG(!script.is_valid(), "Script is not valid.");
		} break;
		case NOTIFICATION_EXIT_TREE: {
		} break;
		case NOTIFICATION_VISIBILITY_CHANGED: {
		} break;
	}
}

void LinkerInspector::_property_changed(const String &p_path, const Variant &p_value, const String &p_name, bool p_changing, bool p_update_all) {
	if (p_value == original_value) {
		return;
	}

	switch (editing_state) {
		case EditingState::Property: {
			original_property_info.type = p_value.get_type();
			if (original_property_info.type == Variant::OBJECT) {
				original_property_info.class_name = Object::cast_to<Object>(p_value)->get_class_name();
			} else {
				original_property_info.class_name = "";
			}
			script->set_member_variable(original_property_info, p_value);
		} break;
		case EditingState::Function: {
		} break;
		default: {
			ERR_FAIL_MSG("Unknown editing type.");
		} break;
	}
}

void LinkerInspector::_property_removed() {
	switch (editing_state) {
		case EditingState::Property: {
			script->remove_property(original_property_info.name);
		} break;
		case EditingState::Function: {
		} break;
		default: {
			ERR_FAIL_MSG("Unknown editing type.");
		} break;
	}
}

void LinkerInspector::_add_close_button(Control *p_editor) {
	Button *close_button = memnew(Button);
	p_editor->add_child(close_button);
	close_button->set_icon(Control::get_theme_icon(SNAME("Close"), EditorStringName(EditorIcons)));
	close_button->connect("pressed", callable_mp((Node *)p_editor, &Node::queue_free));
	close_button->set_tooltip_text("Close" + editing_sufix);
}

void LinkerInspector::_add_delete_button(Control *p_editor) {
	Button *delete_button = memnew(Button);
	p_editor->add_child(delete_button);
	delete_button->set_icon(Control::get_theme_icon(SNAME("Remove"), EditorStringName(EditorIcons)));
	delete_button->connect("pressed", callable_mp(this, &LinkerInspector::_property_removed));
	delete_button->connect("pressed", callable_mp((Node *)p_editor, &Node::queue_free));
	delete_button->set_tooltip_text("Remove" + editing_sufix);
}

void LinkerInspector::_setup_property_editor(EditorProperty *p_editor_property) {
	if (!p_editor_property) {
		return;
	}
	p_editor_property->set_object_and_property(this, StringName("editing_value"));
	p_editor_property->update_property();
	p_editor_property->set_custom_minimum_size(Vector2(50, 10));
	p_editor_property->set_name_split_ratio(0.0); // hide name label
	p_editor_property->connect("focus_exited", callable_mp((Node *)p_editor_property, &Node::queue_free));
	p_editor_property->connect("property_changed", callable_mp(this, &LinkerInspector::_property_changed).bind(false));
}

void LinkerInspector::edit_member(const StringName &p_name) {
	EditorProperty *editor = nullptr;
	VFlowContainer *editor_container;

	if (script->get_property_name_list().find(p_name)) {
		editing_state = EditingState::Property;
		editing_sufix = " property";
		PropertyInfo pi = script->get_property_info(p_name);
		editing_value = script->get_property_default_value(p_name);
		original_value = editing_value;
		original_property_info = pi;
		editor = EditorInspector::instantiate_property_editor(this, pi.type, "editing_value", pi.hint, pi.hint_string, pi.usage);
	}

	if (editor) {
		_setup_property_editor(editor);
		editor_container = memnew(VFlowContainer);
		add_child(editor_container);
		editor_container->set_v_size_flags(SIZE_EXPAND_FILL);
		editor_container->set_h_size_flags(SIZE_EXPAND_FILL);
		//editor_container->set_custom_minimum_size(Vector2(400, 16));
		editor_container->set_global_position(get_global_mouse_position());
		editor_container->add_child(editor);
		_add_close_button(editor_container);
		_add_delete_button(editor_container);
	}
	// instantiate property editor
	// setup and connect editor
	// reposition editor
	// setup quefree on lose focus
}