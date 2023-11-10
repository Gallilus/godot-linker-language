#include "linker_script_instance.h"

bool LinkerScriptInstance::set(const StringName &p_name, const Variant &p_value) {
	return false;
}

bool LinkerScriptInstance::get(const StringName &p_name, Variant &r_ret) const {
	return false;
}

void LinkerScriptInstance::get_property_list(List<PropertyInfo> *p_properties) const {
}

Variant::Type LinkerScriptInstance::get_property_type(const StringName &p_name, bool *r_is_valid) const {
	return Variant::Type();
}

void LinkerScriptInstance::validate_property(PropertyInfo &p_property) const {
}

bool LinkerScriptInstance::property_can_revert(const StringName &p_name) const {
	return false;
}

bool LinkerScriptInstance::property_get_revert(const StringName &p_name, Variant &r_ret) const {
	return false;
}

void LinkerScriptInstance::get_method_list(List<MethodInfo> *p_list) const {
}

bool LinkerScriptInstance::has_method(const StringName &p_method) const {
	return false;
}

Variant LinkerScriptInstance::callp(const StringName &p_method, const Variant **p_args, int p_argcount, Callable::CallError &r_error) {
	return Variant();
}

void LinkerScriptInstance::notification(int p_notification, bool p_reversed) {
}

void LinkerScriptInstance::initialize() {
	if (initialized) {
		return;
	}
	initialized = true;
}

LinkerScriptInstance::LinkerScriptInstance() {
	owner = nullptr;
	base_ref_counted = false;
}

LinkerScriptInstance::~LinkerScriptInstance() {
	MutexLock lock(LinkerLanguage::get_singleton()->mutex);

	if (script.is_valid() && owner) {
		script->instances.erase(owner);
	}
}
