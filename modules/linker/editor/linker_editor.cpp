#include "linker_editor.h"
#include "editor/editor_undo_redo_manager.h"

void LinkerEditor::_update_graph() {
	if (is_updating) {
		return;
	}
	if (script.is_null()) {
		return;
	}
	is_updating = true;
	base_editor->update_graph();
	members_section->update_members();
	is_updating = false;
}

void LinkerEditor::_bind_methods() {
	//ClassDB::bind_method(D_METHOD("script_changed"), &LinkerEditor::script_changed);
}

void LinkerEditor::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			linker_editor_inspector_plugin.instantiate();
			EditorInspector::add_inspector_plugin(linker_editor_inspector_plugin);
		} break;
		case NOTIFICATION_READY: {
			ERR_FAIL_NULL(script);
			ERR_FAIL_COND_MSG(!script.is_valid(), "Script is not valid.");
			script->connect("changed", callable_mp(this, &LinkerEditor::script_changed));
			base_editor->set_script(script);
			members_section->set_script(script);
			inspector->set_script(script);
			selector->set_script(script);
		} break;
		case NOTIFICATION_EXIT_TREE: {
			members_section->queue_free();
			EditorInspector::remove_inspector_plugin(linker_editor_inspector_plugin);
		} break;
		case NOTIFICATION_VISIBILITY_CHANGED: {
			members_section->set_visible(is_visible_in_tree());
		} break;
	}
}

void LinkerEditor::apply_code() {
	// runs on save
	if (script.is_null()) {
		return;
	}
	//	script->set_source_code(code_editor->get_text_editor()->get_text());
	script->update_exports();
	//	code_editor->get_text_editor()->get_syntax_highlighter()->update_cache();
}

Ref<Resource> LinkerEditor::get_edited_resource() const {
	if (script.is_valid()) {
		return script;
	}
	return Ref<Resource>();
}

Vector<String> LinkerEditor::get_functions() {
	List<MethodInfo> functions;
	script->get_script_method_list(&functions);
	Vector<String> ret;
	for (List<MethodInfo>::Element *E = functions.front(); E; E = E->next()) {
		ret.push_back(E->get().name);
	}
	return ret;
}

void LinkerEditor::set_edited_resource(const Ref<Resource> &p_res) {
	ERR_FAIL_COND(script.is_valid());
	ERR_FAIL_NULL(p_res);
	script = p_res;
}

void LinkerEditor::enable_editor(Control *p_shortcut_context) {
	if (editor_enabled) {
		return;
	}
	editor_enabled = true;

	set_focus_mode(Control::FOCUS_ALL);
	set_v_size_flags(SIZE_EXPAND_FILL);
	set_h_size_flags(SIZE_EXPAND_FILL);
	this->connect("ready", callable_mp(this, &LinkerEditor::_update_graph));
	this->connect("resized", callable_mp(this, &LinkerEditor::_update_graph));

	HBoxContainer *top_menu = memnew(HBoxContainer);
	add_child(top_menu);
	Button *test_button = memnew(Button);
	top_menu->add_child(test_button);
	test_button->set_text("test");
	test_button->connect("pressed", callable_mp(this, &LinkerEditor::test));
	Button *_update_graph_button = memnew(Button);
	top_menu->add_child(_update_graph_button);
	_update_graph_button->set_text("update_graph");
	_update_graph_button->connect("pressed", callable_mp(this, &LinkerEditor::_update_graph));

	add_child(base_editor);

	add_child(inspector);
	members_section->connect("edit_member", callable_mp(inspector, &LinkerInspector::edit_member));

	add_child(selector);
	base_editor->connect("inspect_links_request", callable_mp(selector, &LinkerEditorSelector::inspect_links));
}

String LinkerEditor::get_name() {
	String name = "";
	if (script.is_valid()) {
		name = script->get_path().get_file();
		if (is_unsaved()) {
			name += " (*)";
		}
	}
	return name;
}

Ref<Texture2D> LinkerEditor::get_theme_icon() {
	return EditorNode::get_singleton()->get_object_icon(script.ptr(), "GraphEdit");
}

bool LinkerEditor::is_unsaved() {
	bool r = true;
#ifdef TOOLS_ENABLED
	r = script->is_unsaved();
#endif
	return r;
}

Control *LinkerEditor::get_edit_menu() {
	return edit_menu;
}

void LinkerEditor::set_find_replace_bar(FindReplaceBar *p_bar) {
	p_bar = find_replace_bar;
}

Control *LinkerEditor::get_base_editor() const {
	return base_editor;
}

ScriptEditorBase *create_editor(const Ref<Resource> &p_resource) {
	if (Object::cast_to<LinkerScript>(*p_resource)) {
		return memnew(LinkerEditor);
	}
	return nullptr;
}

static void register_editor_callback() {
	ScriptEditor::register_create_script_editor_function(create_editor);
}

void LinkerEditor::register_editor() {
	//ScriptEditor::register_create_script_editor_function(LinkerEditor::create_editor);
	// Too early to register stuff here, request a callback.
	EditorNode::add_plugin_init_callback(register_editor_callback);
}

void LinkerEditor::test() {
	ERR_PRINT("test button pressed");
	script->set_member_variable(PropertyInfo(Variant::STRING, "test"), Variant());

	// EditorUndoRedoManager *undo_redo = EditorUndoRedoManager::get_singleton();
	// // undo_redo->create_action(TTR("test button pressed"), UndoRedo::MERGE_DISABLE, script.ptr());
	// undo_redo->create_action(TTR("test button pressed"));
	// undo_redo->add_do_method(this, "script_changed");
	// undo_redo->add_do_method(script.ptr(), "set_member_variable", PropertyInfo(Variant::STRING, "test2"), Variant());
	// //undo_redo->add_do_method(callable_mp(script.ptr(), &LinkerScript::set_member_variable).bind(PropertyInfo(Variant::STRING, "test2"), Variant()));

	// undo_redo->add_undo_method(script.ptr(), "remove_property", PropertyInfo(Variant::STRING, "test2"));
	// undo_redo->add_undo_method(script.ptr(), "remove_property", PropertyInfo(Variant::STRING, "test"));
	// undo_redo->commit_action();
}

void LinkerEditor::script_changed() {
	if (is_updating) {
		return;
	}
	_update_graph();
}

LinkerEditor::LinkerEditor() {
	base_editor = memnew(LinkerEditorLayout);
	edit_menu = memnew(Control);
	find_replace_bar = memnew(FindReplaceBar);
	members_section = memnew(MembersSection);
	inspector = memnew(LinkerInspector);
	selector = memnew(LinkerEditorSelector);
}