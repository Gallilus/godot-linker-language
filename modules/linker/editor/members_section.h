#ifndef MEMBERS_SECTION_H
#define MEMBERS_SECTION_H

#include "../language/linker_script.h"
#include "plugins/tree_droppable.h"

#include "core/string/string_name.h"
#include "editor/plugins/script_editor_plugin.h"
#include "scene/gui/box_container.h"
#include "scene/gui/control.h"
#include "scene/gui/item_list.h"
#include "scene/gui/split_container.h"
#include "scene/gui/tree.h"

class MembersSection : public VBoxContainer {
	GDCLASS(MembersSection, VBoxContainer);

	Ref<LinkerScript> script;

	const int ID_ADD = 0;
	const int ID_OVERRIDE = 1;
	const int ID_EDIT = 2;

	StringName selected;

	bool updating_members = true;
	TreeDroppable *members = nullptr;
	TreeItem *functions = nullptr;
	TreeItem *variables = nullptr;
	void _update_members();

	void _member_button(Object *p_item, int p_column, int p_button, MouseButton p_mouse_button);
	void _member_edited();
	Dictionary _item_metadata2(const String &p_type, const Variant &p_value) const;
	String _item_meta_name(const Dictionary &p_metadata) const;
	String _item_meta_group(const Dictionary &p_metadata) const;

	PopupMenu *select_virtual_method = nullptr;
	void _on_virtual_method_selected(int p_index);

protected:
	static void _bind_methods();

public:
	void set_script(Ref<LinkerScript> p_script) { script = p_script; }
	void update_members() { _update_members(); }

	virtual Variant get_drag_data(const Point2 &p_point) override;

	MembersSection();
	~MembersSection() {}
};

#endif // MEMBERS_SECTION_H