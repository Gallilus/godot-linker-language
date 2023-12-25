#ifndef LINKER_INSPECTOR_H
#define LINKER_INSPECTOR_H

#include "../language/linker_script.h"
#include "plugins/linker_editor_inspector_plugin.h"

#include "editor/editor_inspector.h"
#include "scene/gui/control.h"
#include "scene/gui/flow_container.h"

class LinkerInspector : public Control {
	GDCLASS(LinkerInspector, Control);
	Ref<LinkerScript> script;

	Variant editing_value;
	Variant original_value;
	PropertyInfo original_property_info;

	enum class EditingState {
		Nothing,
		Property,
		Function
	};

	EditingState editing_state = EditingState::Nothing;
	String editing_sufix = "";

protected:
	static void _bind_methods();
	void _notification(int p_what);

	void _property_changed(const String &p_path, const Variant &p_value, const String &p_name, bool p_changing, bool p_update_all);
	void _property_removed();

	void _add_close_button(Control *p_control);
	void _add_delete_button(Control *p_control);

	void _setup_property_editor(EditorProperty *p_editor_property);

public:
	enum EDIT_TYPE {
		PROPERTY
	};

	struct linker_inspector_data {
		EDIT_TYPE type;
		StringName name;
	};

	void set_editing_value(const Variant &p_value) { editing_value = p_value; }
	Variant get_editing_value() { return editing_value; }

	void set_script(Ref<LinkerScript> p_script) { script = p_script; }
	void edit_member(const StringName &p_name);

	LinkerInspector() {}
	~LinkerInspector() {}
};

#endif // LINKER_INSPECTOR_H
