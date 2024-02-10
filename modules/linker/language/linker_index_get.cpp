#include "linker_index_get.h"

void LinkerIndexGet::_bind_methods() {}

void LinkerIndexGet::_initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_stack_size) {
	LinkerIndexGetInstance *instance = static_cast<LinkerIndexGetInstance *>(link);

	instance->host = p_host;
	instance->index = index;

	instance->pull_count = pull_links.size();
	instance->push_count = push_links.size();

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

Variant LinkerIndexGet::get_placeholder_value() const {
	// get the object index value
	return Variant();
}

Dictionary LinkerIndexGet::get_placeholder_info() const {
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

bool LinkerIndexGet::can_drop(Ref<LinkerLink> drag_link) const {
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

void LinkerIndexGet::drop_data(Ref<LinkerLink> dropped_link) {
	set_source(dropped_link);
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
	if (pull_count > 0) {
		value = pull_links[0]->get_value().get(index);
	}
	return STEP_COMPLETE;
}
