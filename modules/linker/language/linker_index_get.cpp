#include "linker_index_get.h"

void LinkerIndexGet::_bind_methods() {}

void LinkerIndexGet::_initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_stack_size) {
	LinkerIndexGetInstance *instance = static_cast<LinkerIndexGetInstance *>(link);

	instance->host = p_host;
	instance->index = index;

	instance->arg_count = arg_links.size();
	instance->push_count = push_links.size();

	if (object_link.is_valid()) {
		instance->object_link = object_link->get_instance(p_host, p_stack_size);
	}

	for (int i = 0; i < instance->arg_count; i++) {
		LinkerLinkInstance *_link = arg_links[i]->get_instance(p_host, p_stack_size);
		if (_link) {
			instance->arg_links.push_back(_link);
		} else {
			ERR_PRINT(String(arg_links[i]->get_class_name()) + ": instance is null");
		}
	}

	for (int i = 0; i < instance->push_count; i++) {
		LinkerLinkInstance *_link = push_links[i]->get_instance(p_host, p_stack_size);
		if (_link) {
			instance->push_links.push_back(_link);
		} else {
			ERR_PRINT(String(push_links[i]->get_class_name()) + ": instance is null");
		}
	}
}

Variant LinkerIndexGet::get_placeholder_value() const {
	if (object_link != nullptr) {
		return object_link->get_placeholder_value().get(index);
	}
	return Variant();
}

Dictionary LinkerIndexGet::get_placeholder_info() const {
	PropertyInfo pi;
	if (!get_object().is_valid()) {
		return Dictionary();
	}
	pi = get_object()->get_output_info();
	ClassDB::get_property_info(pi.class_name, index, &pi);
	Dictionary d;
	d["type"] = "PropertyInfo";
	d["value"] = Dictionary(pi);
	return d;
}

bool LinkerIndexGet::can_drop_on_link(Ref<LinkerLink> drag_link) const {
	if (drag_link.is_null()) {
		return false;
	}
	if (drag_link->get_class() == "LinkerIndexGet" &&
			drag_link != this) {
		return true;
	}
	if (drag_link->get_class() == "LinkerIndexSet") {
		return false;
	}
	return false;
}

LinkerLinkInstance *LinkerIndexGet::get_instance(LinkerScriptInstance *p_host, int p_stack_size) {
	if (!link_instances.has(p_stack_size)) {
		LinkerIndexGetInstance *instance = new LinkerIndexGetInstance();
		link_instances.insert(p_stack_size, p_host->add_link_instance(instance));
		_initialize_instance(link_instances[p_stack_size], p_host, p_stack_size);
	}
	return link_instances[p_stack_size];
}

void LinkerIndexGet::remove_instance(LinkerScriptInstance *p_host, int p_stack_size) {
	if (link_instances.has(p_stack_size)) {
		p_host->remove_link_instance(link_instances[p_stack_size]);
		link_instances.erase(p_stack_size);
	}
}

////////////////////////////////////////////////////////////////////////
//////////////////////////////  INSTANCE  //////////////////////////////
////////////////////////////////////////////////////////////////////////

int LinkerIndexGetInstance::_step(StartMode p_start_mode, Callable::CallError &r_error, String &r_error_str) {
	// check and use object link
	if (object_link != nullptr) {
		value = object_link->get_value().get(index);
		return STEP_COMPLETE;
	}

	// check and use script property
	bool valid = false;
	Object *object = host->get_owner();
	Variant p_value = object->get(index, &valid);
	if (valid) {
		value = p_value;
		return STEP_COMPLETE;
	} else {
		r_error.error = Callable::CallError::CALL_ERROR_INSTANCE_IS_NULL;
		r_error_str = "LinkerIndexGet " + String(index) + "expects a object link or valid script property";
		return STEP_ERROR;
	}
}
