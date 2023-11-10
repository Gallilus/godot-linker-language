#ifndef LINKER_GRAPH_H
#define LINKER_GRAPH_H

#include "../language/linker_language.h"
#include "../language/linker_script.h"
#include "linker_node.h"

#include "core/object/ref_counted.h"
#include "editor/editor_interface.h"
#include "editor/editor_node.h"
#include "scene/gui/control.h"
#include "scene/gui/grid_container.h"
#include "scene/gui/label.h"
#include "scene/gui/texture_rect.h"
#include "scene/main/window.h"

class LinkerGraph : public Control {
	GDCLASS(LinkerGraph, Control);
	Ref<LinkerScript> script;

	GridContainer *unsorted_nodes = nullptr;

protected:
	static void _bind_methods() {}

public:
	struct DropData {
		bool ctrl_drop = false;
		bool shift_drop = false;
		Variant::Type object_type;
		StringName object_class_name;
		Vector<LinkerScript::NodeInfo> nodes;
		StringName property_name;
		Variant value;
	};

	void set_script(Ref<LinkerScript> p_script) { script = p_script; }

	virtual void gui_input(const Ref<InputEvent> &p_event) override;

	virtual bool can_drop_data(const Point2 &p_point, const Variant &p_data) const override;
	virtual void drop_data(const Point2 &p_point, const Variant &p_data) override;

	void update_graph();

	LinkerGraph();
	~LinkerGraph() {}
};

#endif // LINKER_GRAPH_H