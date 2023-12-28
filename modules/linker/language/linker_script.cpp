#include "linker_script.h"
#include "linker_script_instance.h"

void LinkerScript::set_links(TypedArray<LinkerLink> p_links) {
	for (int i = 0; i < p_links.size(); i++) {
		add_link_by_index(Ref<LinkerLink>(p_links[i]));
	};
}

void LinkerScript::add_link_by_index(Ref<LinkerLink> p_link) {
	if (p_link->get_member_name() != StringName() && has_named_link(p_link->get_member_name())) {
		return;
	}
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

List<StringName> LinkerScript::get_method_name_list() const {
	List<MethodInfo> list;
	get_script_method_list(&list);
	List<StringName> r_list;

	for (int i = 0; i < list.size(); i++) {
		r_list.push_back(list[i].name);
	}
	return r_list;
}

void LinkerScript::set_method_list(const Dictionary &p_methods) {
	for (int i = 0; i < p_methods.keys().size(); i++) {
		StringName key = p_methods.keys()[i];
		set_method(MethodInfo::from_dict(p_methods[key]));
	};
}

void LinkerScript::set_method(const MethodInfo &p_info) {
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

void LinkerScript::rename_method(const StringName &p_name, const StringName &p_new_name) {
	if (member_functions.has(p_new_name)) {
		ERR_PRINT("Duplicate member name: " + p_new_name);
		return;
	}
	if (!member_functions.has(p_name)) {
		ERR_PRINT("No member name: " + p_name);
		return;
	}
	if (member_functions.has(p_name)) {
		MethodInfo info = member_functions[p_name];
		member_functions.erase(p_name);
		member_functions.insert(p_new_name, info);
		emit_changed();
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
		ERR_PRINT("No member name: " + p_name);
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
	ERR_PRINT("No member name: " + p_name);
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

void LinkerScript::add_scene_refrence(Ref<LinkerSceneRefrence> p_node_info) {
	if (!scene_refrences.has(p_node_info->get_node_scene_relative_path())) {
		scene_refrences.insert(p_node_info->get_node_scene_relative_path(), p_node_info);
		emit_changed();
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
	if (p_link->get_member_name() != StringName() && has_named_link(p_link->get_member_name())) {
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

bool LinkerScript::has_named_link(const StringName &p_name) const {
	for (int i = 0; i < links.size(); i++) {
		if (links[i].is_valid() && links[i]->get_member_name() == p_name) {
			return true;
		}
	}
	return false;
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
#ifdef TOOLS_ENABLED
	connect("changed", callable_mp(this, &LinkerScript::set_saved).bind(false));
#endif // TOOLS_ENABLED
}
