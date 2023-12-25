#ifndef LINKER_EDITOR_INSPECTOR_PLUGIN_H
#define LINKER_EDITOR_INSPECTOR_PLUGIN_H

#include "editor_propertie_stringify.h"

#include "editor/editor_inspector.h"

class LinkerEditorInspectorPlugin : public EditorInspectorPlugin {
	GDCLASS(LinkerEditorInspectorPlugin, EditorInspectorPlugin);

public:
	virtual bool can_handle(Object *p_object) override;
	virtual bool parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide = false) override;

	static EditorProperty *get_editor_for_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide = false);
};

#endif // LINKER_EDITOR_INSPECTOR_PLUGIN_H