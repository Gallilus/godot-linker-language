#ifndef EDITOR_LAYOUT_H
#define EDITOR_LAYOUT_H

#include "../language/linker_index_get.h"
#include "../language/linker_language.h"
#include "../language/linker_scene_refrence.h"
#include "../language/linker_script.h"
#include "link_controler.h"

#include "core/object/ref_counted.h"
#include "editor/editor_interface.h"
#include "editor/editor_node.h"
#include "editor_graph.h"
// #include "scene/gui/container.h"
#include "scene/gui/box_container.h"
#include "scene/gui/control.h"
#include "scene/gui/grid_container.h"
#include "scene/gui/label.h"
#include "scene/gui/panel.h"
#include "scene/gui/scroll_container.h"
#include "scene/gui/split_container.h"
#include "scene/gui/texture_rect.h"
#include "scene/main/window.h"

class EditorLayout : public Control {
	GDCLASS(EditorLayout, Control);
	Ref<LinkerScript> script;

	HashMap<LinkerLink *, LinkControler *> link_contorlers;

protected:
	static void _bind_methods();

	LinkControler *get_linker_controler(LinkerLink *p_link);

public:
	struct DropData {
		bool ctrl_drop = false;
		bool shift_drop = false;
		Variant::Type object_type;
		StringName object_class_name;
		Vector<Ref<LinkerSceneRefrence>> nodes;
		StringName property_name;
		Variant value;
	};

	void set_script(Ref<LinkerScript> p_script) { script = p_script; }

	virtual bool can_drop_data(const Point2 &p_point, const Variant &p_data) const override;
	virtual void drop_data(const Point2 &p_point, const Variant &p_data) override;

	void update_graph();

	EditorLayout();
	~EditorLayout() {}
};

#endif // EDITOR_LAYOUT_H
