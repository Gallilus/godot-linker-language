#ifndef EDITOR_LAYOUT_H
#define EDITOR_LAYOUT_H

#include "../language/linker_index_get.h"
#include "../language/linker_language.h"
#include "../language/linker_scene_refrence.h"
#include "../language/linker_script.h"

#include "core/variant/variant_utility.h"
#include "editor/editor_interface.h"
#include "editor/editor_node.h"
#include "scene/gui/box_container.h"
#include "scene/gui/control.h"
#include "scene/gui/grid_container.h"
#include "scene/gui/item_list.h"
#include "scene/gui/label.h"
#include "scene/gui/panel.h"
#include "scene/gui/scroll_container.h"
#include "scene/gui/split_container.h"
#include "scene/gui/texture_rect.h"
#include "scene/gui/tree.h"
#include "scene/main/window.h"

class LinkControler;
class LinkConnection;
class EditorGraph;

class LinkerEditorLayout : public Control {
	GDCLASS(LinkerEditorLayout, Control);
	Ref<LinkerScript> script;
	EditorGraph *_graph = nullptr;

	HashMap<Ref<LinkerLink>, LinkControler *> link_contorlers;
	Vector<LinkConnection *> link_connections;
	HashMap<Ref<LinkerLink>, HashMap<Ref<LinkerLink>, LinkConnection *>> connections_map;

	bool updating_members = true;
	VBoxContainer *members_section = nullptr;
	Tree *members = nullptr;
	void _update_members();

protected:
	static void _bind_methods();

	void _store_connection(Ref<LinkerLink> p_source, Ref<LinkerLink> p_destination, LinkConnection *p_connection);
	bool _has_connection(Ref<LinkerLink> p_source, Ref<LinkerLink> p_destination) const;

	LinkControler *make_link_controler(Ref<LinkerLink> p_link);
	LinkConnection *make_link_connection(Ref<LinkerLink> p_source, Ref<LinkerLink> p_destination);

public:
	struct DropData {
		bool ctrl_drop = false;
		bool shift_drop = false;
		Variant::Type object_type;
		StringName object_class_name;
		Vector<Ref<LinkerSceneRefrence>> nodes;
		Vector<Ref<LinkerLink>> links;
		StringName property_name;
		Variant value;
	};

	void set_script(Ref<LinkerScript> p_script) { script = p_script; }

	virtual void gui_input(const Ref<InputEvent> &p_event) override;
	virtual bool can_drop_data(const Point2 &p_point, const Variant &p_data) const override;
	virtual void drop_data(const Point2 &p_point, const Variant &p_data) override;

	LinkControler *get_link_source_controler(Ref<LinkerLink> p_link);
	LinkControler *get_link_controler(Ref<LinkerLink> p_link);

	void update_graph();
	void add_link(Ref<LinkerLink> p_link);
	void add_pull_connection(Ref<LinkerLink> pulled_link, Ref<LinkerLink> owner_link);
	void add_sequence_connection(Ref<LinkerLink> source_link, Ref<LinkerLink> destination_link);

	LinkerEditorLayout();
	~LinkerEditorLayout() {}
};

#endif // EDITOR_LAYOUT_H
