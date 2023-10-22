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
	;

	// set script refrence for sub editors heare

	// emit signal to update all sub editors
	// same signal as after reload
}

void LinkerEditor::enable_editor(Control *p_shortcut_context) {
	if (editor_enabled) {
		return;
	}
	editor_enabled = true;

	// ToDo set shortcut context

	// initialize sub editors heare

	// emit signal to update all sub editors
	// same signal as after reload
}

String LinkerEditor::get_name() {
	String name = "";
	if (script.is_valid()) {
		name = script->get_path().get_file();
		if (is_unsaved()){
			name += " (*)";
		}
	}
	return name;
}

Ref<Texture2D> LinkerEditor::get_theme_icon() {
	return EditorNode::get_singleton()->get_object_icon(script.ptr(), "GraphEdit");
}

bool LinkerEditor::is_unsaved() {
	// Compare editor with Save
	const bool unsaved = script->get_path().is_empty(); // In memory.
	// Scriptname += (*)
	return unsaved;
}

Control *LinkerEditor::get_edit_menu() {
	return edit_menu;
}

void LinkerEditor::set_find_replace_bar(FindReplaceBar *p_bar) {
	p_bar = find_replace_bar;
}

Control *LinkerEditor::get_base_editor() const {
	Control *bbbb = memnew(Control);
	base_editor->add_child(bbbb);
	return base_editor;
}

ScriptEditorBase *LinkerEditor::create_editor(const Ref<Resource> &p_resource) {
	if (Object::cast_to<LinkerScript>(*p_resource)) {
		return memnew(LinkerEditor);
	}
	return nullptr;
}

static void register_editor_callback() {
	ScriptEditor::register_create_script_editor_function(LinkerEditor::create_editor);
}

void LinkerEditor::register_editor() {
	// Too early to register stuff here, request a callback.
	EditorNode::add_plugin_init_callback(register_editor_callback);
}

LinkerEditor::LinkerEditor() {
	base_editor = memnew(Control);
	edit_menu = memnew(Control);
	find_replace_bar = memnew(FindReplaceBar);
}

LinkerEditor::~LinkerEditor() {
}
