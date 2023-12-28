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
	functions->set_metadata(0, _item_metadata2("group_name", "Functions"));
	functions->add_button(0, Control::get_theme_icon(SNAME("Override"), EditorStringName(EditorIcons)), ID_OVERRIDE, false, TTR("Override an existing built-in function."));
	functions->add_button(0, Control::get_theme_icon(SNAME("Add"), EditorStringName(EditorIcons)), ID_ADD, false, TTR("Create a new function."));
	functions->set_custom_color(0, Control::get_theme_color(SNAME("mono_color"), SNAME("Editor")));

	List<StringName> func_names = script->get_method_name_list();
	func_names.sort_custom<StringName::AlphCompare>();
	for (const StringName &E : func_names) {
		TreeItem *ti = members->create_item(functions);
		MethodInfo mi = script->get_method_info(E);
		ti->set_text(0, E);
		ti->set_selectable(0, true);
		ti->set_metadata(0, _item_metadata2("method", Dictionary(mi)));
		ti->add_button(0, Control::get_theme_icon(SNAME("Edit"), EditorStringName(EditorIcons)), ID_EDIT);
		{
			TreeItem *func_return = members->create_item(ti);
			func_return->set_text(0, "return");
			func_return->set_selectable(0, true);
			func_return->set_metadata(0, _item_metadata2("func_return", E));
			func_return->set_icon(0, Control::get_theme_icon(Variant::get_type_name(mi.return_val.type), SNAME("EditorIcons")));
		}
		for (int i = 0; i < mi.arguments.size(); i++) {
			TreeItem *func_arg = members->create_item(ti);
			func_arg->set_text(0, mi.arguments[i].name);
			func_arg->set_selectable(0, true);
			func_arg->set_metadata(0, _item_metadata2("func_arg", Dictionary(mi.arguments[i])));
			func_arg->set_icon(0, Control::get_theme_icon(Variant::get_type_name(mi.arguments[i].type), SNAME("EditorIcons")));
		}
		ti->set_collapsed(true);
	}

	variables = members->create_item(root);
	variables->set_selectable(0, false);
	variables->set_text(0, TTR("Variables:"));
	variables->set_metadata(0, _item_metadata2("group_name", "Variables"));
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
		ti->set_metadata(0, _item_metadata2("variable", Dictionary(script->get_property_info(E))));
		ti->add_button(0, Control::get_theme_icon(SNAME("Edit"), EditorStringName(EditorIcons)), ID_EDIT);
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

	// Todo in Script: handle errors
	// if (!new_name.is_valid_identifier()) {
	// 	EditorNode::get_singleton()->show_warning(TTR("Name is not a valid identifier:") + " " + new_name);
	// 	updating_members = true;
	// 	ti->set_text(0, name);
	// 	updating_members = false;
	// 	return;
	// }
	// if (script->has_function(new_name) || script->has_variable(new_name) || script->has_custom_signal(new_name)) {
	// 	EditorNode::get_singleton()->show_warning(TTR("Name already in use by another func/var/signal:") + " " + new_name);
	// 	updating_members = true;
	// 	ti->set_text(0, name);
	// 	updating_members = false;
	// 	return;
	// }
	// check not is nodePath

	if (p_button == ID_OVERRIDE) {
		if (group == "group_name" && name == "Functions") {
			select_virtual_method->clear();
			select_virtual_method->popup_centered();
			select_virtual_method->set_max_size(Size2(300, 400));
			select_virtual_method->add_icon_item(Control::get_theme_icon(SNAME("Add"), EditorStringName(EditorIcons)), TTR("Create a new function."));
			select_virtual_method->add_separator();
			select_virtual_method->add_item(TTR("Override Function"));

			List<MethodInfo> methods;
			script->get_script_method_list(&methods);
			//ClassDB::get_method_list(script->get_instance_base_type(), &methods);
			ClassDB::get_virtual_methods("Node", &methods);
			for (List<MethodInfo>::Element *E = methods.front(); E; E = E->next()) {
				select_virtual_method->add_item(E->get().name);
				select_virtual_method->set_item_metadata(-1, Dictionary(E->get()));
			}
			select_virtual_method->connect("id_pressed", callable_mp(this, &MembersSection::_on_virtual_method_selected));

			select_virtual_method->popup_centered_ratio(.5f);
		}
	}

	if (p_button == ID_ADD) {
		if (group == "group_name" && name == "Functions") {
			TreeItem *ti = members->create_item(functions);
			ti->set_metadata(0, _item_metadata2("method", ""));
			ti->set_selectable(0, true);
			ti->set_editable(0, true);
		}
		if (group == "group_name" && name == "Variables") {
			TreeItem *ti = members->create_item(variables);
			ti->set_metadata(0, _item_metadata2("variable", ""));
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

	if (group == "method") {
		if (name == "" && name != new_name) {
			script->set_method(MethodInfo(new_name));
		} else if (name != new_name) {
			script->rename_method(name, new_name);
		}
	}

	if (group == "variable") {
		if (name == "" && name != new_name) {
			script->set_member_variable(PropertyInfo(Variant::NIL, new_name), Variant());
		} else if (name != new_name) {
			script->rename_member_variable(name, new_name);
		}
	}
}

Dictionary MembersSection::_item_metadata2(const String &p_type, const Variant &p_value) const {
	Dictionary d;
	d["type"] = p_type;
	d["value"] = p_value;
	return d;
}

String MembersSection::_item_meta_name(const Dictionary &p_metadata) const {
	Variant value = p_metadata["value"];
	if (value.get_type() == Variant::STRING) {
		return value;
	}
	if (value.get_type() == Variant::DICTIONARY) {
		Dictionary d = value;
		if (d.has("name")) {
			return String(d["name"]);
		}
	}
	return "";
}

String MembersSection::_item_meta_group(const Dictionary &p_metadata) const {
	return p_metadata["type"];
}

void MembersSection::_on_virtual_method_selected(int p_index) {
	MethodInfo mi = MethodInfo::from_dict(select_virtual_method->get_item_metadata(p_index));
	script->set_method(mi);
}

void MembersSection::_bind_methods() {
	ADD_SIGNAL(MethodInfo("edit_member", PropertyInfo(Variant::STRING, "name")));
}

Variant MembersSection::get_drag_data(const Point2 &p_point) {
	ERR_PRINT("get_drag_data " + String(p_point));
	return Variant();
}

MembersSection::MembersSection() {
	ScriptEditor::get_singleton()->get_left_list_split()->call_deferred(("add_child"), this);
	set_v_size_flags(SIZE_EXPAND_FILL);
	set_visible(false);

	// CheckButton *tool_script_check = memnew(CheckButton);
	// tool_script_check->set_text(TTR("Make Tool:"));
	// members_section->add_child(tool_script_check);
	// tool_script_check->connect("pressed", callable_mp(this, &MembersSection::_toggle_tool_script));

	members = memnew(TreeDroppable);
	add_margin_child(TTR("Members:"), members, true);
	members->set_custom_minimum_size(Size2(0, 150));
	members->set_hide_root(true);
	members->set_drop_mode_flags(Tree::DROP_MODE_INBETWEEN | Tree::DROP_MODE_ON_ITEM);
	members->connect("button_clicked", callable_mp(this, &MembersSection::_member_button));
	members->connect("item_edited", callable_mp(this, &MembersSection::_member_edited));
	// members->connect("cell_selected", callable_mp(this, &MembersSection::_member_selected), CONNECT_DEFERRED);
	// members->connect("gui_input", callable_mp(this, &MembersSection::_members_gui_input));
	// members->connect("item_mouse_selected", callable_mp(this, &MembersSection::_member_rmb_selected));
	// members->set_allow_rmb_select(true);
	// members->set_allow_reselect(true);
	// members->set_hide_folding(true);
	// members->set_drag_forwarding(this);

	select_virtual_method = memnew(PopupMenu);
	add_child(select_virtual_method);
}