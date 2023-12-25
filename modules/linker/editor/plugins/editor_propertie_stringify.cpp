#include "editor_propertie_stringify.h"

void EditorPropertyStringify::_text_changed(const String &p_string) {
	Variant new_value = VariantUtilityFunctions::str_to_var(p_string);

	if (updating) {
		return;
	}

	if (new_value.get_type() == Variant::NIL) {
		return;
	}

	emit_changed(get_edited_property(), new_value);
}

void EditorPropertyStringify::_text_submitted(const String &p_string) {
	if (updating) {
		return;
	}

	if (text->has_focus()) {
		text->release_focus();
		_text_changed(p_string);
	}
}

void EditorPropertyStringify::_set_read_only(bool p_read_only) {
	text->set_editable(!p_read_only);
}

void EditorPropertyStringify::update_property() {
	String str = VariantUtilityFunctions::var_to_str(get_edited_property_value());
	updating = true;
	if (text->get_text() != str) {
		int caret = text->get_caret_column();
		text->set_text(str);
		text->set_caret_column(caret);
	}
	text->set_editable(!is_read_only());
	updating = false;
}

void EditorPropertyStringify::set_placeholder(const String &p_string) {
	text->set_placeholder(p_string);
}

EditorPropertyStringify::EditorPropertyStringify() {
	prop_button = memnew(Button);
	prop_button->set_text("<null>");
	prop_button->set_flat(true);
	add_child(prop_button);
	prop_button->connect("pressed", callable_mp(this, &EditorPropertyStringify::start_edit_void));
	text = memnew(LineEdit); // create LineEdit so configfunctions are able to set there stuf
}

EditorPropertyStringify::~EditorPropertyStringify() {
	if (!text->is_inside_tree()) {
		text->queue_free();
	}
}

void EditorPropertyStringify::start_edit_void() {
	prop_button->queue_free();
	HBoxContainer *hb = memnew(HBoxContainer);
	add_child(hb);

	hb->add_child(text);
	add_focusable(text);
	text->set_h_size_flags(SIZE_EXPAND_FILL);
	text->connect("text_changed", callable_mp(this, &EditorPropertyStringify::_text_changed));
	text->connect("text_submitted", callable_mp(this, &EditorPropertyStringify::_text_submitted));
}