#include "linker_editor.h"

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
	ERR_FAIL_COND(p_res.is_null());
	script = p_res;

	// set script refrence for sub editors heare

	// emit signal to update all sub editors
	// same signal as after reload
}

void LinkerEditor::enable_editor(Control *p_shortcut_context) {
	if (editor_enabled) {
		return;
	}
	editor_enabled = true;

	set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);

	HBoxContainer *top_menu = memnew(HBoxContainer);
	top_menu->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	add_child(top_menu);
	Button *test_button = memnew(Button);
	test_button->set_text("test");
	top_menu->add_child(test_button);
	test_button->connect("pressed", callable_mp(this, &LinkerEditor::test));

	add_child(base_editor);
	base_editor->set_focus_mode(Control::FOCUS_ALL);
	base_editor->set_v_size_flags(SIZE_EXPAND_FILL);

	Panel *background = memnew(Panel);
	background->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	background->set_modulate(Color(0.1, 1.0, 0.1));
	base_editor->add_child(background);

	// ToDo set shortcut context

	// initialize sub editors heare

	// emit signal to update all sub editors
	// same signal as after reload
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
	script->set_member_variable("test", PropertyInfo(Variant::STRING, "test"), nullptr);
}

LinkerEditor::LinkerEditor() {
	base_editor = memnew(Control);
	edit_menu = memnew(Control);
	find_replace_bar = memnew(FindReplaceBar);
}

LinkerEditor::~LinkerEditor() {
	if (!editor_enabled) {
		memdelete(base_editor);
		memdelete(edit_menu);
		memdelete(find_replace_bar);
	}
}
