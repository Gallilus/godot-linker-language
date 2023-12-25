#include "members_section.h"
#include "editor/editor_string_names.h"

void MembersSection::_update_members() {
	ERR_FAIL_COND(!script.is_valid());
	updating_members = true;

	members->clear();
	TreeItem *root = members->create_item();

	functions = members->create_item(root);
	functions->set_selectable(0, false);
	functions->set_text(0, TTR("Functions:"));
	functions->set_metadata(0, _item_metadata("Functions", "group_name"));
	functions->add_button(0, Control::get_theme_icon(SNAME("Override"), EditorStringName(EditorIcons)), ID_OVERRIDE, false, TTR("Override an existing built-in function."));
	functions->add_button(0, Control::get_theme_icon(SNAME("Add"), EditorStringName(EditorIcons)), ID_ADD, false, TTR("Create a new function."));
	functions->set_custom_color(0, Control::get_theme_color(SNAME("mono_color"), SNAME("Editor")));

	List<StringName> func_names = script->get_method_name_list();
	func_names.sort_custom<StringName::AlphCompare>();
	for (const StringName &E : func_names) {
		TreeItem *ti = members->create_item(functions);
		ti->set_text(0, E);
		ti->set_selectable(0, true);
		ti->set_metadata(0, _item_metadata(E, "Functions"));
		ti->add_button(0, Control::get_theme_icon(SNAME("Edit"), EditorStringName(EditorIcons)), ID_EDIT);
		if (selected == E) {
			ti->select(0);
		}
	}

	variables = members->create_item(root);
	variables->set_selectable(0, false);
	variables->set_text(0, TTR("Variables:"));
	variables->set_metadata(0, _item_metadata("Variables", "group_name"));
	variables->add_button(0, Control::get_theme_icon(SNAME("Add"), EditorStringName(EditorIcons)), ID_ADD, false, TTR("Create a new variable."));
	variables->set_custom_color(0, Control::get_theme_color(SNAME("mono_color"), SNAME("Editor")));

	List<StringName> var_names = script->get_property_name_list();
	var_names.sort_custom<StringName::AlphCompare>();
	for (const StringName &E : var_names) {
		TreeItem *ti = members->create_item(variables);

		ti->set_text(0, E);

		String icon_name = Variant::get_type_name(script->get_property_info(E).type);
		ti->set_icon(0, Control::get_theme_icon(icon_name, SNAME("EditorIcons")));

		ti->set_selectable(0, true);
		ti->set_metadata(0, _item_metadata(E, "Variables"));
		ti->add_button(0, Control::get_theme_icon(SNAME("Edit"), EditorStringName(EditorIcons)), ID_EDIT);
		if (selected == E) {
			ti->select(0);
		}
	}

	updating_members = false;
}

void MembersSection::_member_button(Object *p_item, int p_column, int p_button, MouseButton p_mouse_button) {
	if (p_mouse_button != MouseButton::LEFT) {
		return;
	}

	TreeItem *ti = Object::cast_to<TreeItem>(p_item);
	TreeItem *root = members->get_root();

	String name = _item_meta_name(ti->get_metadata(0));
	String group = _item_meta_group(ti->get_metadata(0));

	if (p_button == ID_OVERRIDE) {
		if (group == "group_name" && name == "Functions") {
			// emit signal
			ERR_PRINT("Override function");
		}
	}

	if (p_button == ID_ADD) {
		if (group == "group_name" && name == "Functions") {
			TreeItem *ti = members->create_item(functions);
			ti->set_metadata(0, _item_metadata("", "Functions"));
			ti->set_selectable(0, true);
			ti->set_editable(0, true);
		}
		if (group == "group_name" && name == "Variables") {
			TreeItem *ti = members->create_item(variables);
			ti->set_metadata(0, _item_metadata("", "Variables"));
			ti->set_selectable(0, true);
			ti->set_editable(0, true);
		}
	}

	if (p_button == ID_EDIT) {
		emit_signal("edit_member", name);
	}
}

void MembersSection::_member_edited() {
	if (updating_members) {
		return;
	}

	TreeItem *ti = members->get_edited();
	ERR_FAIL_COND(!ti);

	String name = _item_meta_name(ti->get_metadata(0));
	String group = _item_meta_group(ti->get_metadata(0));

	String new_name = ti->get_text(0);

	if (name == new_name) {
		return;
	}

	if (group == "group_name") {
		return;
	}

	if (group == "Functions") {
		if (name == "" && new_name != "") {
			script->set_method(MethodInfo(new_name));
		} else if (name != new_name) {
			script->rename_method(name, new_name);
		}
	}

	if (group == "Variables") {
		ERR_PRINT(String(new_name));
		if (name == "" && name != new_name) {
			script->set_member_variable(PropertyInfo(Variant::NIL, new_name), Variant());
		} else if (name != new_name) {
			script->rename_method(name, new_name);
		}
	}
}

Dictionary MembersSection::_item_metadata(const String &p_name, const String &p_group) const {
	Dictionary d;
	d["name"] = p_name;
	d["group"] = p_group;
	return d;
}

String MembersSection::_item_meta_name(const Dictionary &p_metadata) const {
	return p_metadata["name"];
}

String MembersSection::_item_meta_group(const Dictionary &p_metadata) const {
	return p_metadata["group"];
}

void MembersSection::_bind_methods() {
	ADD_SIGNAL(MethodInfo("edit_member", PropertyInfo(Variant::STRING, "name")));
}

MembersSection::MembersSection() {
	ScriptEditor::get_singleton()->get_left_list_split()->call_deferred(("add_child"), this);
	set_v_size_flags(SIZE_EXPAND_FILL);
	set_visible(false);

	// CheckButton *tool_script_check = memnew(CheckButton);
	// tool_script_check->set_text(TTR("Make Tool:"));
	// members_section->add_child(tool_script_check);
	// tool_script_check->connect("pressed", callable_mp(this, &MembersSection::_toggle_tool_script));

	members = memnew(Tree);
	add_margin_child(TTR("Members:"), members, true);
	members->set_custom_minimum_size(Size2(0, 150));
	members->set_hide_root(true);
	members->connect("button_clicked", callable_mp(this, &MembersSection::_member_button));
	members->connect("item_edited", callable_mp(this, &MembersSection::_member_edited));
	// members->connect("cell_selected", callable_mp(this, &MembersSection::_member_selected), CONNECT_DEFERRED);
	// members->connect("gui_input", callable_mp(this, &MembersSection::_members_gui_input));
	// members->connect("item_mouse_selected", callable_mp(this, &MembersSection::_member_rmb_selected));
	// members->set_allow_rmb_select(true);
	// members->set_allow_reselect(true);
	// members->set_hide_folding(true);
	// members->set_drag_forwarding(this);
}