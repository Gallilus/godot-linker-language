#include "linker_language.h"

#include "core/config/engine.h"
#include "core/core_constants.h"
#include "core/io/file_access.h"

#ifdef TOOLS_ENABLED
#include "core/config/project_settings.h"
#include "editor/editor_file_system.h"
#include "editor/editor_settings.h"
#endif

LinkerLanguage *LinkerLanguage::singleton = nullptr;

bool LinkerLanguage::debug_break(const String &p_error, bool p_allow_continue) {
	if (EngineDebugger::is_active() && Thread::get_caller_id() == Thread::get_main_id()) {
		// _debug_parse_err_node = -1;
		// _debug_parse_err_file = "";
		// _debug_error = p_error;
		EngineDebugger::get_script_debugger()->debug(this, p_allow_continue, true);
		return true;
	} else {
		return false;
	}
}

String LinkerLanguage::get_name() const {
	return "LinkerScript";
}

String LinkerLanguage::get_type() const {
	return "LinkerScript";
}

String LinkerLanguage::get_extension() const {
	return "ls";
}

void LinkerLanguage::get_reserved_words(List<String> *p_words) const {
	// Please keep alphabetical order within categories.
	static const char *_reserved_words[] = {
		// Control flow.
		"break",
		"continue",
		"elif",
		"else",
		"for",
		"if",
		"match",
		"pass",
		"return",
		"when",
		"while",
		// Declarations.
		"class",
		"class_name",
		"const",
		"enum",
		"extends",
		"func",
		"namespace", // Reserved for potential future use.
		"signal",
		"static",
		"trait", // Reserved for potential future use.
		"var",
		// Other keywords.
		"await",
		"breakpoint",
		"self",
		"super",
		"yield", // Reserved for potential future use.
		// Operators.
		"and",
		"as",
		"in",
		"is",
		"not",
		"or",
		// Special values (tokenizer treats them as literals, not as tokens).
		"false",
		"null",
		"true",
		// Constants.
		"INF",
		"NAN",
		"PI",
		"TAU",
		// Functions (highlighter uses global function color instead).
		"assert",
		"preload",
		// Types (highlighter uses type color instead).
		"void",
		nullptr,
	};

	const char **w = _reserved_words;

	while (*w) {
		p_words->push_back(*w);
		w++;
	}
}

bool LinkerLanguage::is_control_flow_keyword(String p_keyword) const {
	// Please keep alphabetical order.
	return p_keyword == "break" ||
			p_keyword == "continue" ||
			p_keyword == "elif" ||
			p_keyword == "else" ||
			p_keyword == "for" ||
			p_keyword == "if" ||
			p_keyword == "match" ||
			p_keyword == "pass" ||
			p_keyword == "return" ||
			p_keyword == "when" ||
			p_keyword == "while";
}

void LinkerLanguage::get_comment_delimiters(List<String> *p_delimiters) const {
	p_delimiters->push_back("#");
}

void LinkerLanguage::get_doc_comment_delimiters(List<String> *p_delimiters) const {
	p_delimiters->push_back("##");
}

void LinkerLanguage::get_string_delimiters(List<String> *p_delimiters) const {
	p_delimiters->push_back("\" \"");
	p_delimiters->push_back("' '");
	p_delimiters->push_back("\"\"\" \"\"\"");
	p_delimiters->push_back("''' '''");
	// NOTE: StringName, NodePath and r-strings are not listed here.
}

Ref<Script> LinkerLanguage::make_template(const String &p_template, const String &p_class_name, const String &p_base_class_name) const {
	Ref<LinkerScript> scr;
	scr.instantiate();
	String processed_template = p_template;
	bool type_hints = false;
#ifdef TOOLS_ENABLED
	type_hints = EDITOR_GET("text_editor/completion/add_type_hints");
#endif
	if (!type_hints) {
		processed_template = processed_template.replace(": int", "")
									 .replace(": Shader.Mode", "")
									 .replace(": VisualShader.Type", "")
									 .replace(": float", "")
									 .replace(": String", "")
									 .replace(": Array[String]", "")
									 .replace(": Node", "")
									 .replace(": CharFXTransform", "")
									 .replace(":=", "=")
									 .replace(" -> void", "")
									 .replace(" -> bool", "")
									 .replace(" -> int", "")
									 .replace(" -> PortType", "")
									 .replace(" -> String", "")
									 .replace(" -> Object", "");
	}

	processed_template = processed_template.replace("_BASE_", p_base_class_name)
								 .replace("_CLASS_SNAKE_CASE_", p_class_name.to_snake_case().validate_identifier())
								 .replace("_CLASS_", p_class_name.to_pascal_case().validate_identifier());
	scr->set_source_code(processed_template);
	return scr;
}

Vector<ScriptLanguage::ScriptTemplate> LinkerLanguage::get_built_in_templates(const StringName &p_object) {
	Vector<ScriptLanguage::ScriptTemplate> templates;
#ifdef TOOLS_ENABLED
	/*for (int i = 0; i < TEMPLATES_ARRAY_SIZE; i++) {
		if (TEMPLATES[i].inherit == p_object) {
			// ToDo check if template is for LinkerScript
			// templates.append(TEMPLATES[i]);
		}
	}*/
#endif
	return templates;
}

bool LinkerLanguage::validate(const String &p_script, const String &p_path, List<String> *r_functions, List<ScriptError> *r_errors, List<Warning> *r_warnings, HashSet<int> *r_safe_lines) const {
	return false;
}

Script *LinkerLanguage::create_script() const {
	return memnew(LinkerScript);
}

void LinkerLanguage::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back(get_extension());
}

void LinkerLanguage::add_register_func(const String &p_name, LinkerLinkRegisterFunc p_func) {
	ERR_FAIL_COND(registered_links.has(p_name));
	registered_links[p_name] = p_func;
}

void LinkerLanguage::remove_register_func(const String &p_name) {
	ERR_FAIL_COND(!registered_links.has(p_name));
	registered_links.erase(p_name);
}

void LinkerLanguage::get_registered_link_names(List<String> *r_names) const {
	for (const KeyValue<String, LinkerLinkRegisterFunc> &E : registered_links) {
		r_names->push_back(E.key);
	}
}

LinkerLanguage::LinkerLanguage() {
	singleton = this;
}

LinkerLanguage::~LinkerLanguage() {
	singleton = nullptr;
}
