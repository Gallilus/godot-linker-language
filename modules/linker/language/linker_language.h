#ifndef LINKER_SCRIPT_LANGUAGE_H
#define LINKER_SCRIPT_LANGUAGE_H

#include "linker_script.h"

#include "core/debugger/engine_debugger.h"
#include "core/debugger/script_debugger.h"
#include "core/object/script_language.h"
#include "core/templates/self_list.h"

using namespace godot;

typedef Ref<LinkerLink> (*LinkerLinkRegisterFunc)(const String &p_type);

class LinkerLanguage : public ScriptLanguage {
	HashMap<String, LinkerLinkRegisterFunc> register_funcs;

	friend class LinkerScriptInstance;
	static LinkerLanguage *singleton;

	Mutex mutex;

	friend class LinkerScript;
	SelfList<LinkerScript>::List script_list;

	uint64_t script_frame_time;

public:
	bool debug_break(const String &p_error, bool p_allow_continue = true);

	virtual String get_name() const override;
	_FORCE_INLINE_ static LinkerLanguage *get_singleton() { return singleton; }

	/* LANGUAGE FUNCTIONS */
	virtual void init() override {}
	virtual String get_type() const override;
	virtual String get_extension() const override;
	virtual void finish() override {}

	/* EDITOR FUNCTIONS */
	virtual void get_reserved_words(List<String> *p_words) const override;
	virtual bool is_control_flow_keyword(String p_string) const override;
	virtual void get_comment_delimiters(List<String> *p_delimiters) const override;
	virtual void get_doc_comment_delimiters(List<String> *p_delimiters) const override;
	virtual void get_string_delimiters(List<String> *p_delimiters) const override;
	virtual Ref<Script> make_template(const String &p_template, const String &p_class_name, const String &p_base_class_name) const override;
	virtual Vector<ScriptTemplate> get_built_in_templates(const StringName &p_object) override;
	virtual bool is_using_templates() { return false; }
	virtual bool validate(const String &p_script, const String &p_path = "", List<String> *r_functions = nullptr, List<ScriptError> *r_errors = nullptr, List<Warning> *r_warnings = nullptr, HashSet<int> *r_safe_lines = nullptr) const override;
	//virtual String validate_path(const String &p_path) const { return ""; }
	virtual Script *create_script() const override;
#ifndef DISABLE_DEPRECATED
	virtual bool has_named_classes() const override { return false; }
#endif
	virtual bool supports_builtin_mode() const override { return false; }
	virtual bool supports_documentation() const override { return false; }
	virtual bool can_inherit_from_file() const override { return false; }
	virtual int find_function(const String &p_function, const String &p_code) const override { return -1; }
	virtual String make_function(const String &p_class, const String &p_name, const PackedStringArray &p_args) const override { return String(); }
	virtual Error open_in_external_editor(const Ref<Script> &p_script, int p_line, int p_col) override { return ERR_UNAVAILABLE; }
	virtual bool overrides_external_editor() override { return false; }

	virtual Error lookup_code(const String &p_code, const String &p_symbol, const String &p_path, Object *p_owner, LookupResult &r_result) override { return ERR_UNAVAILABLE; }

	virtual void auto_indent_code(String &p_code, int p_from_line, int p_to_line) const override {}
	virtual void add_global_constant(const StringName &p_variable, const Variant &p_value) override {}
	virtual void add_named_global_constant(const StringName &p_name, const Variant &p_value) override {}
	virtual void remove_named_global_constant(const StringName &p_name) override {}

	virtual String debug_get_error() const override { return String(); }
	virtual int debug_get_stack_level_count() const override { return -1; }
	virtual int debug_get_stack_level_line(int p_level) const override { return -1; }
	virtual String debug_get_stack_level_function(int p_level) const override { return String(); }
	virtual String debug_get_stack_level_source(int p_level) const override { return String(); }
	virtual void debug_get_stack_level_locals(int p_level, List<String> *p_locals, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1) override {}
	virtual void debug_get_stack_level_members(int p_level, List<String> *p_members, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1) override {}
	virtual ScriptInstance *debug_get_stack_level_instance(int p_level) { return nullptr; }
	virtual void debug_get_globals(List<String> *p_globals, List<Variant> *p_values, int p_max_subitems = -1, int p_max_depth = -1) override {}
	virtual String debug_parse_stack_level_expression(int p_level, const String &p_expression, int p_max_subitems = -1, int p_max_depth = -1) override { return String(); }

	virtual Vector<StackInfo> debug_get_current_stack_info() { return Vector<StackInfo>(); }

	virtual void reload_all_scripts() override {}
	virtual void reload_scripts(const Array &p_scripts, bool p_soft_reload) override {} // new in 4.3
	virtual void reload_tool_script(const Ref<Script> &p_script, bool p_soft_reload) override {}
	/* LOADER FUNCTIONS */

	virtual void get_recognized_extensions(List<String> *p_extensions) const override;
	virtual void get_public_functions(List<MethodInfo> *p_functions) const override {}
	virtual void get_public_constants(List<Pair<String, Variant>> *p_constants) const override {}
	virtual void get_public_annotations(List<MethodInfo> *p_annotations) const override {}

	virtual void profiling_start() override {}
	virtual void profiling_stop() override {}
	virtual void profiling_set_save_native_calls(bool p_enable) override {} // new in 4.3

	virtual int profiling_get_accumulated_data(ProfilingInfo *p_info_arr, int p_info_max) override { return 0; }
	virtual int profiling_get_frame_data(ProfilingInfo *p_info_arr, int p_info_max) override { return 0; }

	virtual void frame() override {}

	virtual bool handles_global_class_type(const String &p_type) const { return false; }
	virtual String get_global_class_name(const String &p_path, String *r_base_type = nullptr, String *r_icon_path = nullptr) const { return String(); }

	void add_register_func(const String &p_name, LinkerLinkRegisterFunc p_func);
	void remove_register_func(const String &p_name);

	LinkerLanguage();
	~LinkerLanguage();
};
#endif // LINKER_SCRIPT_LANGUAGE_H