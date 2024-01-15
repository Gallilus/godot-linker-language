#include "linker_index_get.h"

void LinkerIndexGet::_bind_methods() {}

void LinkerIndexGet::_initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_stack_size) {
	LinkerIndexGetInstance *instance = static_cast<LinkerIndexGetInstance *>(link);

	instance->host = p_host;
	instance->index = index;

	for (int i = 0; i < pull_links.size(); i++) {
		LinkerLinkInstance *_link = pull_links[i]->get_instance(p_host, p_stack_size);
		if (_link) {
			instance->pull_links.push_back(_link);
		} else {
			ERR_PRINT(String(pull_links[i]->get_class_name()) + ": instance is null");
		}
	}
	for (int i = 0; i < push_links.size(); i++) {
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
	// prefix dictionary with type PropertyInfo
	// keys (type, value)
	return Dictionary(/*get the object index info*/);
}

bool LinkerIndexGet::can_drop(Ref<LinkerLink> drag_link) const {
	if (drag_link.is_null()) {
		return false;
	}
	if (drag_link->get_class() == "LinkerIndexGet") {
		return true;
	}
	if (drag_link->get_class() == "LinkerIndexSet" &&
			drag_link != this) {
		return true;
	}
	return false;
}

void LinkerIndexGet::drop_data(Ref<LinkerLink> dropped_link) {
	if (dropped_link.is_null()) {
		return;
	}
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

int LinkerIndexGetInstance::step(StartMode p_start_mode, Callable::CallError &r_error, String &r_error_str) {
	return 0;
}
