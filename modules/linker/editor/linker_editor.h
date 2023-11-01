#ifndef LINKER_EDITOR_H
#define LINKER_EDITOR_H

#include "core/object/ref_counted.h"
#include "editor/code_editor.h"
#include "editor/editor_node.h"
#include "editor/editor_string_names.h"
#include "editor/plugins/script_editor_plugin.h"
#include "scene/gui/control.h"

#include "../language/linker_language.h"
#include "../language/linker_script.h"

class LinkerEditor : public ScriptEditorBase {
	GDCLASS(LinkerEditor, ScriptEditorBase);

	Ref<LinkerScript> script;

	bool editor_enabled = false;

	Control *edit_menu = nullptr;
	FindReplaceBar *find_replace_bar = nullptr;
	Control *base_editor = nullptr;

protected:
	static void _bind_methods() {}

public:
	virtual void add_syntax_highlighter(Ref<EditorSyntaxHighlighter> p_highlighter) override {}
	virtual void set_syntax_highlighter(Ref<EditorSyntaxHighlighter> p_highlighter) override {}

	virtual void apply_code() override;
	virtual Ref<Resource> get_edited_resource() const override;
	virtual Vector<String> get_functions() override;
	virtual void set_edited_resource(const Ref<Resource> &p_res) override;
	virtual void enable_editor(Control *p_shortcut_context = nullptr) override;
	virtual void reload_text() {}
	virtual String get_name() override;
	virtual Ref<Texture2D> get_theme_icon() override;
	virtual bool is_unsaved() override;
	virtual Variant get_edit_state() override { return Variant(); }
	virtual void set_edit_state(const Variant &p_state) override {}
	virtual Variant get_navigation_state() override { return Variant(); }
	virtual void goto_line(int p_line, bool p_with_error = false) override {}
	virtual void set_executing_line(int p_line) override {}
	virtual void clear_executing_line() override {}
	virtual void trim_trailing_whitespace() override {}
	virtual void insert_final_newline() override {}
	virtual void convert_indent() override {}
	virtual void ensure_focus() override { base_editor->grab_focus(); }
	virtual void tag_saved_version() override {}
	virtual void reload(bool p_soft) { ERR_PRINT("reload " + itos(p_soft)); }
	virtual PackedInt32Array get_breakpoints() override { return PackedInt32Array(); }
	virtual void set_breakpoint(int p_line, bool p_enabled) override {}
	virtual void clear_breakpoints() override {}
	virtual void add_callback(const String &p_function, PackedStringArray p_args) override {}
	virtual void update_settings() override {}
	virtual void set_debugger_active(bool p_active) override {}
	virtual bool can_lose_focus_on_node_selection() { return true; }
	virtual void update_toggle_scripts_button() {}

	virtual bool show_members_overview() override { return true; }

	virtual void set_tooltip_request_func(const Callable &p_toolip_callback) override {}

	virtual Control *get_edit_menu() override;
	virtual void clear_edit_menu() override {}

	virtual void set_find_replace_bar(FindReplaceBar *p_bar) override;

	virtual Control *get_base_editor() const override;
	virtual void validate() override {}

	static void register_editor();

	void test();

	LinkerEditor();
	~LinkerEditor();
};

#endif // LINKER_EDITOR_H