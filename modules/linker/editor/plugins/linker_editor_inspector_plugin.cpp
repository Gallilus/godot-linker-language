#include "linker_editor_inspector_plugin.h"

bool LinkerEditorInspectorPlugin::can_handle(Object *p_object) {
	return false; // can handle everything for now LinkerInspector never cals can_handle so this is not needed
}

bool LinkerEditorInspectorPlugin::parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide) {
	if (p_object->get_class_name() != "LinkerInspector") {
		return false; // can handle only LinkerInspector
	}
	Control *editor = LinkerEditorInspectorPlugin::get_editor_for_property(p_object, p_type, p_path, p_hint, p_hint_text, p_usage, p_wide);
	if (editor) {
		add_property_editor(p_path, editor);
	}
	return true;
}

EditorProperty *LinkerEditorInspectorPlugin::get_editor_for_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide) {
	switch (p_type) {
		case Variant::NIL: {
			EditorPropertyStringify *editor = memnew(EditorPropertyStringify);
			editor->set_placeholder("type value here");
			return editor;
		} break;
	}
	return nullptr;
}
