#ifndef LINKER_INSPECTOR_2_H
#define LINKER_INSPECTOR_2_H

#include "../language/linker_link.h"
#include "../language/linker_script.h"
#include "plugins/linker_editor_inspector_plugin.h"

#include "editor/editor_inspector.h"
#include "scene/gui/control.h"
#include "scene/gui/flow_container.h"

class LinkerInspector2 : public Control {
	GDCLASS(LinkerInspector2, Control);
	Variant editing_value;
	Ref<LinkerLink> link;

protected:
	static void _bind_methods();
	void _notification(int p_what) {}

	void property_changed(const String &p_path, const Variant &p_value, const String &p_name, bool p_changing, bool p_update_all);
	void property_removed() {}

	void add_close_button(Control *p_editor) {}
	void add_delete_button(Control *p_editor) {}

	void setup_property_editor(EditorProperty *p_editor_property);

public:
	void set_editing_value(const Variant &p_value) { editing_value = p_value; }
	Variant get_editing_value() { return editing_value; }

	void set_object(Ref<LinkerLink> p_link);
	void popup(Point2 p_pos);

	LinkerInspector2() {}
	~LinkerInspector2() {}
};

#endif // LINKER_INSPECTOR_2_H
