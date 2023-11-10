#include "linker_script.h"
#include "linker_script_instance.h"

ScriptInstance *LinkerScript::_create_instance(const Variant **p_args, int p_argcount, Object *p_owner, bool p_is_ref_counted, Callable::CallError &r_error) {
	// TODO suport creating scripts with initialization arguments
	/* STEP 1, CREATE */
	LinkerScriptInstance *instance = memnew(LinkerScriptInstance);
	instance->base_ref_counted = p_is_ref_counted;
	instance->members.resize(members.size());
	instance->script = Ref<Script>(this);
	instance->owner = p_owner;
	instance->owner_id = p_owner->get_instance_id();

	instance->owner->set_script_instance(instance);
	/* STEP 2, INITIALIZE AND CONSTRUCT */
	{
		MutexLock lock(LinkerLanguage::singleton->mutex);
		instances.insert(instance->owner);
	}
	instance->initialize();
	return instance;
}

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
	LinkerScript *top = this;
	while (top->_base) {
		top = top->_base;
	}

	Callable::CallError unchecked_error;
	return _create_instance(nullptr, 0, p_this, Object::cast_to<RefCounted>(p_this) != nullptr, unchecked_error);
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

TypedArray<StringName> LinkerScript::get_members() const {
	TypedArray<StringName> names;
	for (const StringName &E : members) {
		names.push_back(E);
	}
	return names;
}

Dictionary LinkerScript::get_method_list() const {
	Dictionary methods;
	for (const KeyValue<StringName, MethodInfo> &E : member_functions) {
		methods[E.key] = Dictionary(E.value);
	}
	return methods;
}

void LinkerScript::set_method_list(const Dictionary &p_methods) {
	for (int i = 0; i < p_methods.keys().size(); i++) {
		StringName key = p_methods.keys()[i];
		set_method(MethodInfo(p_methods[key]));
	};
}

void LinkerScript::set_method(const MethodInfo &p_info) {
	if (members.has(p_info.name)) {
		if (member_functions.has(p_info.name)) {
			member_functions[p_info.name] = p_info;
			return;
		}
		ERR_PRINT("Duplicate member name: " + p_info.name);
		return;
	} else {
		members.insert(p_info.name);
		member_functions.insert(p_info.name, p_info);
		return;
	}
}

Dictionary LinkerScript::get_property_list() const {
	Dictionary properties;
	for (const KeyValue<StringName, VariableInfo> &E : member_properties) {
		properties[E.key] = Dictionary(E.value);
	}
	return properties;
}

void LinkerScript::set_property_list(const Dictionary &p_properties) {
	for (int i = 0; i < p_properties.keys().size(); i++) {
		StringName key = p_properties.keys()[i];
		set_property(VariableInfo::from_dict(p_properties[key]));
	};
}

void LinkerScript::set_member_variable(const StringName &p_name, const PropertyInfo &p_info, Variant *p_default_value) {
#ifdef TOOLS_ENABLED
	is_saved = false;
#endif // TOOLS_ENABLED

	if (member_properties.has(p_name)) {
		if (p_default_value == nullptr) {
			member_properties[p_name] = VariableInfo(p_info);
			return;
		}
		member_properties[p_name] = VariableInfo(p_info, &p_default_value);
		return;
	} else if (!members.has(p_name)) {
		members.insert(p_name);
		if (p_default_value == nullptr) {
			member_properties[p_name] = VariableInfo(p_info);
			return;
		}
		member_properties.insert(p_name, VariableInfo(p_info, &p_default_value));
		return;
	}
	ERR_PRINT("Duplicate member variable: " + p_name);
}

void LinkerScript::set_property(const VariableInfo &p_info) {
	if (members.has(p_info.info.name)) {
		if (member_properties.has(p_info.info.name)) {
			member_properties[p_info.info.name] = p_info;
			return;
		}
		ERR_PRINT("Duplicate member name: " + p_info.info.name);
		return;
	} else {
		members.insert(p_info.info.name);
		member_properties.insert(p_info.info.name, p_info);
		return;
	}
}

Dictionary LinkerScript::get_constants() const {
	Dictionary consts;
	for (const KeyValue<StringName, Variant> &E : constants) {
		consts[E.key] = E.value;
	}
	return consts;
}

void LinkerScript::set_constants(const Dictionary &p_constants) {
	for (int i = 0; i < p_constants.keys().size(); i++) {
		StringName key = p_constants.keys()[i];
		set_constant(key, p_constants[key]);
	};
}

void LinkerScript::set_constant(const StringName &p_name, const Variant &p_value) {
	if (members.has(p_name)) {
		if (constants.has(p_name)) {
			constants[p_name] = p_value;
			return;
		}
		ERR_PRINT("Duplicate member name: " + p_name);
		return;
	} else {
		members.insert(p_name);
		constants.insert(p_name, p_value);
		return;
	}
}

Dictionary LinkerScript::get_signal_list() const {
	Dictionary signal_list;
	for (const KeyValue<StringName, MethodInfo> &E : signals) {
		signal_list[E.key] = Dictionary(E.value);
	}
	return signal_list;
}

void LinkerScript::set_signal_list(const Dictionary &p_signals) {
	for (int i = 0; i < p_signals.keys().size(); i++) {
		StringName key = p_signals.keys()[i];
		set_signal(MethodInfo(p_signals[key]));
	};
}

void LinkerScript::set_signal(const MethodInfo &p_info) {
	if (members.has(p_info.name)) {
		if (signals.has(p_info.name)) {
			signals[p_info.name] = p_info;
			return;
		}
		ERR_PRINT("Duplicate member name: " + p_info.name);
		return;
	} else {
		members.insert(p_info.name);
		signals.insert(p_info.name, p_info);
		return;
	}
}

Dictionary LinkerScript::get_scene_refrences() const {
	Dictionary refrence_list;
	for (const KeyValue<StringName, NodeInfo> &E : scene_refrences) {
		refrence_list[E.key] = Dictionary(E.value);
	}
	return refrence_list;
}

void LinkerScript::set_scene_refrences(const Dictionary &p_scene_refrences) {
	for (int i = 0; i < p_scene_refrences.keys().size(); i++) {
		NodePath key = p_scene_refrences.keys()[i];
		add_scene_refrence(NodeInfo::from_dict(p_scene_refrences[key]));
	};
}

void LinkerScript::add_scene_refrence(const NodeInfo &p_node_info) {
	if (!scene_refrences.has(p_node_info.node_scene_relative_path)) {
		scene_refrences.insert(p_node_info.node_scene_relative_path, p_node_info);
	}
	emit_changed();
}

void LinkerScript::overwrite_scene_refrence(const NodeInfo &p_node_info) {
	if (scene_refrences.has(p_node_info.node_scene_relative_path)) {
		scene_refrences.insert(p_node_info.node_scene_relative_path, p_node_info);
	}
	emit_changed();
}

void LinkerScript::remove_scene_refrence(StringName relative_path) {
	if (scene_refrences.has(relative_path)) {
		scene_refrences.erase(relative_path);
	}
	emit_changed();
}

LinkerScript::LinkerScript() :
		script_list(this) {
	{
		MutexLock lock(LinkerLanguage::singleton->mutex);
		LinkerLanguage::get_singleton()->script_list.add(&script_list);
	}
	path = vformat("linkerscript://%d.ls", get_instance_id());
}
