#include "linker_script.h"
#include "linker_language.h"

Ref<Script> LinkerScript::get_base_script() const {
	if (_base) {
		return Ref<Script>(_base); // Try to inherit from any script not only LinkScript or GDScript (delete this comment if inheritnace works)
	} else {
		return Ref<Script>();
	}
}

StringName LinkerScript::get_global_name() const {
	return global_name;
}

bool LinkerScript::inherits_script(const Ref<Script> &p_script) const {
	if (p_script.is_null()) {
		return false;
	}

	const Script *s = this; // Try to inherit from any script not only LinkScript or GDScript (delete this comment if inheritnace works)

	while (s) {
		if (s == p_script.ptr()) {
			return true;
		}
		s = s->get_base_script().ptr();
	}

	return false;
}

StringName LinkerScript::get_instance_base_type() const {
	/*	if (native.is_valid()) {               // ToDo: implement `Ref<LinkerScriptNativeClass> native;`
			return native->get_name();
		} */
	if (base.is_valid() && base->is_valid()) {
		return base->get_instance_base_type();
	}
	return StringName();
}

ScriptInstance *LinkerScript::instance_create(Object *p_this) {
	// ToDo
	return nullptr;
}

bool LinkerScript::instance_has(const Object *p_this) const {
	// MutexLock lock(GDScriptLanguage::singleton->mutex);
	return instances.has((Object *)p_this);
}

bool LinkerScript::has_source_code() const {
	return !source.is_empty();
}

String LinkerScript::get_source_code() const {
	// Create or get source code
	return source;
}

void LinkerScript::set_source_code(const String &p_code) {
	if (source != p_code) {
		source = p_code;
	}
}

Error LinkerScript::reload(bool p_keep_state) {
	if (reloading) {
		return OK;
	}
	//ToDo
	return OK;
}

PropertyInfo LinkerScript::get_class_category() const {
	StringName scr_name;
	if (get_name().is_empty()) {
		scr_name = path.get_file();
	} else {
		scr_name = get_name();
	}

	return PropertyInfo(Variant::NIL, scr_name, PROPERTY_HINT_NONE, path, PROPERTY_USAGE_CATEGORY, global_name);
}

bool LinkerScript::has_method(const StringName &p_method) const {
	return member_functions.has(p_method);
}

bool LinkerScript::has_static_method(const StringName &p_method) const {
	return member_functions.has(p_method) && member_functions[p_method].flags & METHOD_FLAG_STATIC;
}

MethodInfo LinkerScript::get_method_info(const StringName &p_method) const {
	if (!member_functions.has(p_method)) {
		return MethodInfo();
	}

	return member_functions.get(p_method);
	//return static_cast<MethodInfo>(member_functions.get(p_method));
}

ScriptLanguage *LinkerScript::get_language() const {
	return LinkerLanguage::get_singleton();
}

bool LinkerScript::has_script_signal(const StringName &p_signal) const {
	if (signals.has(p_signal)) {
		return true;
	}
	if (base.is_valid()) {
		return base->has_script_signal(p_signal);
	}
	return false;
}

void LinkerScript::get_script_signal_list(List<MethodInfo> *r_signals) const {
	for (const KeyValue<StringName, MethodInfo> &E : signals) {
		r_signals->push_back(E.value);
	}
}

bool LinkerScript::get_property_default_value(const StringName &p_property, Variant &r_value) const {
#ifdef TOOLS_ENABLED
	/*
	HashMap<StringName, Variant>::ConstIterator E = member_default_values_cache.find(p_property);
	if (E) {
		r_value = E->value;
		return true;
	}
	*/
#endif
	return false;
}

void LinkerScript::get_script_method_list(List<MethodInfo> *p_list) const {
	for (const KeyValue<StringName, MethodInfo> &E : member_functions) {
		p_list->push_back(E.value);
	}
}

void LinkerScript::get_script_property_list(List<PropertyInfo> *p_list) const {
	for (const KeyValue<StringName, VariableInfo> &E : member_properties) {
		p_list->push_back(E.value.info);
	}
}

void LinkerScript::get_constants(HashMap<StringName, Variant> *p_constants) {
	if (p_constants) {
		for (const KeyValue<StringName, Variant> &E : constants) {
			(*p_constants)[E.key] = E.value;
		}
	}
}

void LinkerScript::get_members(HashSet<StringName> *p_members) {
	if (p_members) {
		for (const StringName &E : members) {
			p_members->insert(E);
		}
	}
}

void LinkerScript::set_member_variable(const StringName &p_name, const PropertyInfo &p_info, Variant *p_default_value) {
	if (member_properties.has(p_name)) {
		member_properties[p_name] = VariableInfo(p_info, &p_default_value);
		return;
	} else if (!members.has(p_name)) {
		members.insert(p_name);
		member_properties.insert(p_name, VariableInfo(p_info, &p_default_value));
		return;
	}
	ERR_PRINT("Duplicate member variable: " + p_name);
}
