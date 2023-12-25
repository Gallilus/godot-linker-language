#ifndef EDITOR_PROPERTY_STRINGIFY_H
#define EDITOR_PROPERTY_STRINGIFY_H

#include "editor/editor_inspector.h"
#include "editor/editor_node.h"
#include "editor/editor_properties.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/label.h"
#include "scene/gui/line_edit.h"
#include "core/variant/variant_utility.h"

class EditorPropertyStringify : public EditorProperty {
	GDCLASS(EditorPropertyStringify, EditorProperty);
	Button *prop_button = nullptr;
	LineEdit *text = nullptr;

	bool updating = false;
	bool string_name = false;
	void _text_changed(const String &p_string); // on_text_changed check validity and visualise result
	void _text_submitted(const String &p_string); // on_entered_pressed and valid return close editor

protected:
	virtual void _set_read_only(bool p_read_only) override;
	static void _bind_methods() {}

public:
	virtual void update_property() override;
	void set_placeholder(const String &p_string);

	EditorPropertyStringify();
	~EditorPropertyStringify();
	void start_edit_void();
};

#endif // EDITOR_PROPERTY_STRINGIFY_H