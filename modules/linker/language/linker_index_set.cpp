#include "linker_index_set.h"

void LinkerIndexSet::_bind_methods() {
}

void LinkerIndexSet::_initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_stack_size) {
	LinkerIndexSetInstance *instance = static_cast<LinkerIndexSetInstance *>(link);

	instance->host = p_host;
	instance->index = index;

	instance->pull_count = pull_links.size();
	instance->push_count = push_links.size();

	if (source_link.is_valid()) {
		instance->source_link = source_link->get_instance(p_host, p_stack_size);
	}

	for (int i = 0; i < instance->pull_count; i++) {
		LinkerLinkInstance *_link = pull_links[i]->get_instance(p_host, p_stack_size);
		if (_link) {
			instance->pull_links.push_back(_link);
		} else {
			ERR_PRINT(String(pull_links[i]->get_class_name()) + ": instance is null");
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

Variant LinkerIndexSet::get_placeholder_value() const {
	return Variant();
}

Dictionary LinkerIndexSet::get_placeholder_info() const {
	PropertyInfo pi;
	if (!get_source().is_valid()) {
		return Dictionary();
	}
	pi = get_source()->get_output_info();
	ClassDB::get_property_info(pi.class_name, index, &pi);
	Dictionary d;
	d["type"] = "PropertyInfo";
	d["value"] = Dictionary(pi);
	return d;
}

bool LinkerIndexSet::can_drop_on_link(Ref<LinkerLink> drag_link) const {
	// check obj aka source is input or output
	return true;
}

LinkerLinkInstance *LinkerIndexSet::get_instance(LinkerScriptInstance *p_host, int p_stack_size) {
	if (!link_instances.has(p_stack_size)) {
		LinkerIndexSetInstance *instance = new LinkerIndexSetInstance();
		link_instances.insert(p_stack_size, p_host->add_link_instance(instance));
		_initialize_instance(link_instances[p_stack_size], p_host, p_stack_size);
	}
	return link_instances[p_stack_size];
}

void LinkerIndexSet::remove_instance(LinkerScriptInstance *p_host, int p_stack_size) {
	if (link_instances.has(p_stack_size)) {
		p_host->remove_link_instance(link_instances[p_stack_size]);
		link_instances.erase(p_stack_size);
	}
}

////////////////////////////////////////////////////////////////////////
//////////////////////////////  INSTANCE  //////////////////////////////
////////////////////////////////////////////////////////////////////////

int LinkerIndexSetInstance::_step(StartMode p_start_mode, Callable::CallError &r_error, String &r_error_str) {
	value = Variant();
	Variant set_value;

	if (pull_count > 0) {
		set_value = pull_links[0]->get_value();
	} else {
		r_error.error = Callable::CallError::CALL_ERROR_INSTANCE_IS_NULL;
		r_error_str = "LinkerIndexSet " + String(index) + "expects a arg link";
		return STEP_ERROR;
	}

	// check and use source link
	if (source_link != nullptr) {
		source_link->get_value().set(index, set_value);
		return STEP_COMPLETE;
	}

	// check and use script property
	bool valid = false;
	Object *object = host->get_owner();
	object->set(index, set_value, &valid);
	if (valid) {
		return STEP_COMPLETE;
	} else {
		r_error.error = Callable::CallError::CALL_ERROR_INSTANCE_IS_NULL;
		r_error_str = "LinkerIndexSet " + String(index) + "expects a source link or valid script property";
		return STEP_ERROR;
	}
}