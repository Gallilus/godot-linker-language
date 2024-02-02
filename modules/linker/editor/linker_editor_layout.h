#ifndef EDITOR_LAYOUT_H
#define EDITOR_LAYOUT_H

#include "../language/linker_index_get.h"
#include "../language/linker_language.h"
#include "../language/linker_scene_refrence.h"
#include "../language/linker_script.h"

#include "core/variant/variant_utility.h"
#include "editor/editor_interface.h"
#include "editor/editor_node.h"
#include "editor/themes/editor_scale.h"
#include "scene/animation/tween.h"
#include "scene/gui/box_container.h"
#include "scene/gui/control.h"
#include "scene/gui/grid_container.h"
#include "scene/gui/item_list.h"
#include "scene/gui/label.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/margin_container.h"
#include "scene/gui/menu_button.h"
#include "scene/gui/panel.h"
#include "scene/gui/scroll_container.h"
#include "scene/gui/split_container.h"
#include "scene/gui/texture_rect.h"
#include "scene/gui/tree.h"
#include "scene/main/window.h"

class LinkControler;
class LinkConnection;
class EditorGraph;
class ConnectNext;

class LinkerEditorLayout : public Control {
	GDCLASS(LinkerEditorLayout, Control);
	Ref<LinkerScript> script;
	EditorGraph *_graph = nullptr;

	HashMap<Ref<LinkerLink>, LinkControler *> link_contorlers;
	HashMap<Ref<LinkerLink>, HashMap<Ref<LinkerLink>, Vector<LinkConnection *>>> connections_map;

	bool updating_members = true;
	VBoxContainer *members_section = nullptr;
	Tree *members = nullptr;
	ConnectNext *connect_next = nullptr;
	void _update_members();

protected:
	static void _bind_methods();

	LinkControler *make_link_controler(Ref<LinkerLink> p_link);

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
	LinkConnection *get_link_connection(Ref<LinkerLink> source_link, Ref<LinkerLink> destination_link, int p_connection_type);

	void update_graph();
	void add_link(Ref<LinkerLink> p_link);
	void add_pull_connection(Ref<LinkerLink> pulled_link, Ref<LinkerLink> owner_link);
	void add_sequence_connection(Ref<LinkerLink> source_link, Ref<LinkerLink> destination_link);

	LinkerEditorLayout();
	~LinkerEditorLayout() {}
};

class ResultTree : public Tree {
	GDCLASS(ResultTree, Tree);
	friend class ConnectNext;

	int scope_flags = SCOPE_ALL;
	int search_flags = SEARCH_ALL;

	Vector<PropertyInfo> source_info;
	String search_term = "";
	String class_hint = "Node";

protected:
	static void _bind_methods() {}
	void _notification(int p_what) {}

public:
	enum ScopeFlags {
		SCOPE_BASE = 1 << 0,
		SCOPE_INHERITERS = 1 << 1,
		SCOPE_UNRELATED = 1 << 2,
		SCOPE_RELATED = SCOPE_BASE | SCOPE_INHERITERS,
		SCOPE_ALL = SCOPE_BASE | SCOPE_INHERITERS | SCOPE_UNRELATED
	};

	enum SearchFlags {
		SEARCH_CLASSES = 1 << 0,
		SEARCH_CONSTRUCTORS = 1 << 1,
		SEARCH_METHODS = 1 << 2,
		SEARCH_OPERATORS = 1 << 3,
		SEARCH_SIGNALS = 1 << 4,
		SEARCH_CONSTANTS = 1 << 5,
		SEARCH_PROPERTIES = 1 << 6,
		SEARCH_THEME_ITEMS = 1 << 7,
		SEARCH_ALL = SEARCH_CLASSES | SEARCH_CONSTRUCTORS | SEARCH_METHODS |
				SEARCH_OPERATORS | SEARCH_SIGNALS | SEARCH_CONSTANTS |
				SEARCH_PROPERTIES | SEARCH_THEME_ITEMS,
		SEARCH_INSIDE_CLASS = SEARCH_CONSTRUCTORS | SEARCH_METHODS |
				SEARCH_OPERATORS | SEARCH_SIGNALS | SEARCH_CONSTANTS |
				SEARCH_PROPERTIES,
		SEARCH_EXLUDE_FROM_PROPERTIES = 1 << 28,
		SEARCH_CASE_SENSITIVE = 1 << 29,
		SEARCH_SHOW_HIERARCHY = 1 << 30,
	};

	void add_source_info(const PropertyInfo &p_info);
	void set_case_sensitive(bool p_case_sensitive);
	void set_show_hierarchy(bool p_show_hierarchy);
	void set_show_setters_getters(bool p_show_setters_getters);

	void update_results(const String &p_search_term);
	void update_results();

	ResultTree();
	~ResultTree() {}
};

class ConnectNext : public VBoxContainer {
	GDCLASS(ConnectNext, VBoxContainer);

	enum SourceFlags {
		SOURCE_NONE = 0,
		SOURCE_BUILT_IN = 1 << 0,
		SOURCE_HOST = 1 << 1,
		SOURCE_LINK = 1 << 2,
		SOURCE_ALL = SOURCE_BUILT_IN | SOURCE_HOST | SOURCE_LINK
	};

	bool debug_mouse_inside = true;
	bool mouse_inside = true;

	HBoxContainer *menu_bar = nullptr;
	Button *close_button = nullptr;
	MenuButton *source_menu = nullptr;
	int source_flags = SOURCE_NONE;
	MenuButton *scope_menu = nullptr;
	int scope_flags = ResultTree::SCOPE_ALL;
	MenuButton *search_menu = nullptr;
	int search_flags = ResultTree::SEARCH_ALL;

	void source_flag_pressed(int p_flag);
	void refresh_source_menu();
	void scope_flag_pressed(int p_flag);
	void refresh_scope_menu();
	void search_flag_pressed(int p_flag);
	void refresh_search_menu();
	void popup_closed();

	LineEdit *search_text = nullptr;
	ResultTree *results_tree = nullptr;

protected:
	static void _bind_methods() {}
	void _notification(int p_what);

	void _update_icons();
	void _draw_debug();
	void _update_results(const String &p_search_term);
	void _class_from_search_therm();

public:
	void dropped(Ref<LinkerLink> p_link, const Point2 &p_point);
	void popup(const Vector2 &p_pos);
	void close();

	ConnectNext();
	~ConnectNext();
};

#endif // EDITOR_LAYOUT_H