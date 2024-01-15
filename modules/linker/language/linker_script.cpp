#include "linker_script.h"
#include "linker_language.h"

void LinkerScript::set_links(TypedArray<LinkerLink> p_links) {
	for (int i = 0; i < p_links.size(); i++) {
		add_link_by_index(Ref<LinkerLink>(p_links[i]));
	};
}

void LinkerScript::add_link_by_index(Ref<LinkerLink> p_link) {
	if (get_link_idx(p_link.ptr()) < 0) {
		int index = p_link->saved_links_idx;
		links.set(index, p_link);
		p_link->set_host(this);
		emit_changed();
	}
	add_member_link(p_link);
}

void LinkerScript::add_member_link(Ref<LinkerLink> p_link) {
	if (Object::cast_to<LinkerSceneRefrence>(*p_link)) {
		add_scene_refrence(Object::cast_to<LinkerSceneRefrence>(*p_link));
	}
	if (Object::cast_to<LinkerFunction>(*p_link)) {
		add_function_refrence(Object::cast_to<LinkerFunction>(*p_link));
	}
}

void LinkerScript::remove_member_link(Ref<LinkerLink> p_link) {
	if (Object::cast_to<LinkerSceneRefrence>(*p_link)) {
		remove_scene_refrence(Object::cast_to<LinkerSceneRefrence>(*p_link)->get_node_scene_relative_path());
	}
}

int LinkerScript::get_link_count() {
	// ToDo : clean up duplicates and redirect destinations
	for (int i = links.size() - 1; i >= 0; i--) {
		if (links[i].is_null()) {
			links.remove_at(i);
		}
	}
	return links.size();
}

void LinkerScript::init_links_refrences() {
	for (int i = 0; i < links.size(); i++) {
		if (links[i].is_valid()) {
			links[i]->set_link_refrences();
		}
	}
}

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

void LinkerScript::_set_script_loading() {
	script_loading = true;
}

void LinkerScript::_set_script_loaded() {
	script_loading = false;
	set_saved(true);
}

void LinkerScript::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_POSTINITIALIZE: {
#ifdef TOOLS_ENABLED
			connect("changed", callable_mp(this, &LinkerScript::set_saved).bind(false));
#endif // TOOLS_ENABLED
			break;
		}
	}
}

bool LinkerScript::_is_memeber_link(Ref<LinkerLink> p_link) const {
	if (Object::cast_to<LinkerSceneRefrence>(*p_link)) {
		if (scene_refrences.has(p_link->get_index())) {
			return true;
		}
	}
	if (Object::cast_to<LinkerFunction>(*p_link)) {
		if (function_refrences.has(p_link->get_index())) {
			return true;
		}
	}
	return false;
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

Dictionary LinkerScript::get_function_list() const {
	Dictionary methods;
	for (const KeyValue<StringName, MethodInfo> &E : member_functions) {
		methods[E.key] = Dictionary(E.value);
	}
	return methods;
}

Dictionary LinkerScript::get_member_list() const {
	// Todo same as get_function_list only need to filter out private functions and functions linkt to the script not the class
	Dictionary methods;
	for (const KeyValue<StringName, MethodInfo> &E : member_functions) {
		methods[E.key] = Dictionary(E.value);
	}
	return methods;
}

List<StringName> LinkerScript::get_function_name_list() const {
	List<MethodInfo> list;
	get_script_method_list(&list);
	List<StringName> r_list;

	for (int i = 0; i < list.size(); i++) {
		r_list.push_back(list[i].name);
	}
	return r_list;
}

void LinkerScript::set_function_list(const Dictionary &p_methods) {
	for (int i = 0; i < p_methods.keys().size(); i++) {
		StringName key = p_methods.keys()[i];
		set_function(MethodInfo::from_dict(p_methods[key]));
	};
}

void LinkerScript::set_function(const MethodInfo &p_info) {
	if (members.has(p_info.name)) {
		if (member_functions.has(p_info.name)) {
			member_functions[p_info.name] = p_info;
			emit_changed();
			return;
		}
		ERR_PRINT("Duplicate member name: " + p_info.name);
		return;
	} else {
		members.insert(p_info.name);
		member_functions.insert(p_info.name, p_info);
		emit_changed();
		return;
	}
}

void LinkerScript::rename_function(const StringName &p_name, const StringName &p_new_name) {
	if (member_functions.has(p_new_name)) {
		ERR_PRINT("Duplicate method name: " + p_new_name);
		return;
	}
	if (!member_functions.has(p_name)) {
		ERR_PRINT("No method named: " + p_name);
		return;
	}
	if (member_functions.has(p_name)) {
		MethodInfo info = member_functions[p_name];
		member_functions.erase(p_name);
		member_functions.insert(p_new_name, info);
		emit_changed();
		return;
	}
	ERR_PRINT("No method named: " + p_name);
}

Ref<LinkerLink> LinkerScript::get_function_link(const StringName &p_method) {
	if (!members.has(p_method) || !member_functions.has(p_method)) {
		ERR_PRINT("No method named: " + p_method);
		return nullptr;
	}

	for (int i = 0; i < links.size(); i++) {
		if (links[i].is_valid() &&
				links[i]->get_class() == "LinkerFunction" &&
				links[i]->get_index() == p_method) {
			return links[i];
		}
	}
	Ref<LinkerFunction> func = memnew(LinkerFunction);
	func->set_index(p_method);
	return func;
}

void LinkerScript::add_function_refrence(Ref<LinkerFunction> p_link) {
	if (!function_refrences.has(p_link->get_index())) {
		function_refrences.insert(p_link->get_index(), p_link);
	}
}

Dictionary LinkerScript::get_property_list() const {
	Dictionary properties;
	for (const KeyValue<StringName, VariableInfo> &E : member_properties) {
		properties[E.key] = Dictionary(E.value);
	}
	return properties;
}

List<StringName> LinkerScript::get_property_name_list() const {
	List<PropertyInfo> list;
	get_script_property_list(&list);
	List<StringName> r_list;

	for (int i = 0; i < list.size(); i++) {
		r_list.push_back(list[i].name);
	}
	return r_list;
}

void LinkerScript::set_property_list(const Dictionary &p_properties) {
	for (int i = 0; i < p_properties.keys().size(); i++) {
		StringName key = p_properties.keys()[i];
		set_property(VariableInfo::from_dict(p_properties[key]));
	};
}

void LinkerScript::set_property(const VariableInfo &p_info) {
	if (members.has(p_info.info.name)) {
		if (member_properties.has(p_info.info.name)) {
			member_properties[p_info.info.name] = p_info;
			emit_changed();
			return;
		}
		ERR_PRINT("Duplicate member name: " + p_info.info.name);
		return;
	} else if (!members.has(p_info.info.name)) {
		members.insert(p_info.info.name);
		member_properties.insert(p_info.info.name, p_info);
		emit_changed();
		return;
	}
}

void LinkerScript::remove_property(const StringName &p_name) {
	if (members.has(p_name) && member_properties.has(p_name)) {
		members.erase(p_name);
		member_properties.erase(p_name);
		emit_changed();
	}
}

void LinkerScript::set_member_variable(const PropertyInfo &p_info, const Variant &p_default_value) {
	StringName p_name = p_info.name;
	if (member_properties.has(p_name)) {
		member_properties[p_name] = VariableInfo(p_info, p_default_value);
		emit_changed();
		return;
	} else if (!members.has(p_name)) {
		members.insert(p_name);
		member_properties.insert(p_name, VariableInfo(p_info, p_default_value));
		emit_changed();
		return;
	}
}

void LinkerScript::rename_member_variable(const StringName &p_name, const StringName &p_new_name) {
	if (member_properties.has(p_new_name)) {
		ERR_PRINT("Duplicate member name: " + p_new_name);
		return;
	}
	if (!member_properties.has(p_name)) {
		ERR_PRINT("No member named: " + p_name);
		return;
	}
	if (member_properties.has(p_name)) {
		VariableInfo info = member_properties[p_name];
		member_properties.erase(p_name);
		info.info.name = p_new_name;
		member_properties.insert(p_new_name, info);
		emit_changed();
		return;
	}
	ERR_PRINT("No member named: " + p_name);
}

PropertyInfo LinkerScript::get_property_info(const StringName &p_name) {
	if (member_properties.has(p_name)) {
		return member_properties[p_name].info;
	}
	return PropertyInfo();
}

Variant LinkerScript::get_property_default_value(const StringName &p_name) {
	if (member_properties.has(p_name)) {
		return member_properties[p_name].default_value;
	}
	return Variant();
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

TypedArray<LinkerSceneRefrence> LinkerScript::get_scene_refrences() const {
	TypedArray<LinkerSceneRefrence> refrence_list;
	for (const KeyValue<StringName, Ref<LinkerSceneRefrence>> &E : scene_refrences) {
		refrence_list.push_back(E.value);
	}
	return refrence_list;
}

Ref<LinkerSceneRefrence> LinkerScript::get_scene_refrence(const StringName p_relative_path) const {
	if (scene_refrences.has(p_relative_path)) {
		return scene_refrences[p_relative_path];
	}
	return nullptr;
}

void LinkerScript::set_scene_refrences(TypedArray<LinkerSceneRefrence> p_scene_refrences) {
	for (int i = 0; i < p_scene_refrences.size(); i++) {
		add_scene_refrence(Ref<LinkerSceneRefrence>(p_scene_refrences[i]));
	};
}

void LinkerScript::add_scene_refrence(Ref<LinkerSceneRefrence> p_link) {
	if (!scene_refrences.has(p_link->get_index())) {
		scene_refrences.insert(p_link->get_index(), p_link);
	}
}

void LinkerScript::remove_scene_refrence(StringName relative_path) {
	if (scene_refrences.has(relative_path)) {
		links.erase(scene_refrences[relative_path]);
		scene_refrences.erase(relative_path);
		emit_changed();
	}
}

TypedArray<LinkerLink> LinkerScript::get_links() const {
	TypedArray<LinkerLink> link_list;
	for (int i = 0; i < links.size(); i++) {
		link_list.push_back(links[i]);
	}
	return link_list;
}

void LinkerScript::add_link(Ref<LinkerLink> p_link) {
	if (_is_memeber_link(p_link)) {
		return;
	}
	if (get_link_idx(p_link.ptr()) < 0) {
		links.append(p_link);
		p_link->set_host(this);
		emit_changed();
	}
	add_member_link(p_link);
}

void LinkerScript::remove_link(Ref<LinkerLink> p_link) {
	remove_member_link(p_link);
	if (links.has(p_link)) {
		links.erase(p_link);
		// do i need to remove from named links?
		emit_changed();
	}
}

int LinkerScript::get_link_idx(const LinkerLink *p_link) const {
	for (int i = 0; i < links.size(); i++) {
		if (links[i].ptr() == p_link) {
			return i;
		}
	}
	return -1;
}

LinkerScript::LinkerScript() :
		script_list(this) {
	{
		MutexLock lock(LinkerLanguage::singleton->mutex);
		LinkerLanguage::get_singleton()->script_list.add(&script_list);
	}
	path = vformat("linkerscript://%d.ls", get_instance_id());
}

////////////////////////////////////////////////////////////////////////
//////////////////////////////  INSTANCE  //////////////////////////////
////////////////////////////////////////////////////////////////////////

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
	r_error.error = Callable::CallError::CALL_OK; //ok by default

	if (!script->has_method(p_method) ||
			!script->function_refrences.has(p_method)) {
		r_error.error = Callable::CallError::CALL_ERROR_INVALID_METHOD;
		return Variant();
	}
	int total_stack_size = 0;

	return _call_internal(p_method, total_stack_size, 0, 0, false, r_error);
	//return Variant();
}

Variant LinkerScriptInstance::_call_internal(const StringName &p_method, int p_stack_size, int p_flow_stack_pos, int p_pass, bool p_resuming_yield, Callable::CallError &r_error) {
	bool error = false;
	String error_str;
	Variant return_value;

	int current_node_id;

	while (true) {
		p_pass++;

		current_node_id = script->function_refrences[p_method]->get_link_idx();

		LinkerLinkInstance::StartMode start_mode;
		{
			if (p_resuming_yield) {
				start_mode = LinkerLinkInstance::START_MODE_RESUME_YIELD;
				p_resuming_yield = false; // Should resume only the first time.
				// } else ToDo = LinkerLinkInstance::START_MODE_CONTINUE_SEQUENCE;
			} else {
				start_mode = LinkerLinkInstance::START_MODE_BEGIN_SEQUENCE;
			}
		}

		LinkerLinkInstance *link = script->function_refrences[p_method]->get_instance(this, p_stack_size);
		LinkerFunctionInstance *function_instance = static_cast<LinkerFunctionInstance *>(link);
		if (function_instance == nullptr) {
			error_str = "LinkerScriptInstance::_call_internal: instance is not a LinkerFunctionInstance";
			error = true;
			break;
		}

		LinkerLinkInstance::StepResultMask ret; // ToDo = link->step(start_mode, r_error, error_str);

		if (r_error.error != Callable::CallError::CALL_OK) {
			// error from previus step.
			error = true;
			break;
		}

		LinkerLinkInstance::StepResultMask output = static_cast<LinkerLinkInstance::StepResultMask>(ret & LinkerLinkInstance::STEP_MASK);

		if (ret & LinkerLinkInstance::STEP_EXIT_FUNCTION_BIT) {
			return_value; // = ToDo link->get_return_value();
		}
		break;
	}
	if (error) {
		// Error
		// Function, file, line, error, explanation.
		String err_file = script->get_path();
		String err_func = p_method;
		int err_line = current_node_id; // Not a line but it works as one.

		if (r_error.error != Callable::CallError::CALL_ERROR_INVALID_METHOD || error_str.is_empty()) {
			if (!error_str.is_empty()) {
				error_str += " ";
			}

			if (r_error.error == Callable::CallError::CALL_ERROR_INVALID_ARGUMENT) {
				int errorarg = r_error.argument;
				error_str += "Cannot convert argument " + itos(errorarg + 1) + " to " + Variant::get_type_name(Variant::Type(r_error.expected)) + ".";
			} else if (r_error.error == Callable::CallError::CALL_ERROR_TOO_MANY_ARGUMENTS) {
				error_str += "Expected " + itos(r_error.argument) + " arguments.";
			} else if (r_error.error == Callable::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS) {
				error_str += "Expected " + itos(r_error.argument) + " arguments.";
			} else if (r_error.error == Callable::CallError::CALL_ERROR_INVALID_METHOD) {
				error_str += "Invalid Call.";
			} else if (r_error.error == Callable::CallError::CALL_ERROR_METHOD_NOT_CONST) {
				error_str += "Method not const in a const instance.";
			} else if (r_error.error == Callable::CallError::CALL_ERROR_INSTANCE_IS_NULL) {
				error_str += "Base Instance is null";
			}
		}

		// ToDo if (!LinkerLanguage::singleton->debug_break(error_str, false)) {
		_err_print_error(err_func.utf8().get_data(), err_file.utf8().get_data(), err_line, error_str.utf8().get_data(), false, ERR_HANDLER_SCRIPT);
	}

	return return_value;
}

ScriptLanguage *LinkerScriptInstance::get_language() {
	return LinkerLanguage::get_singleton();
}

LinkerLinkInstance *LinkerScriptInstance::add_link_instance(LinkerLinkInstance *p_link_instance) {
	link_instances.push_back(p_link_instance);
	return link_instances.get(link_instances.size() - 1);
}

void LinkerScriptInstance::remove_link_instance(LinkerLinkInstance *p_link_instance) {
	if (!link_instances.has(p_link_instance)) {
		return;
	}
	link_instances.erase(p_link_instance);
	memdelete(p_link_instance);
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
