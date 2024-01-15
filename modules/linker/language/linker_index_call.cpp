#include "linker_index_call.h"

void LinkerIndexCall::_initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_stack_size) {
	LinkerIndexCallInstance *instance = static_cast<LinkerIndexCallInstance *>(link);

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

bool LinkerIndexCall::can_drop(Ref<LinkerLink> drag_link) const {
	if (drag_link.is_null()) {
		return false;
	}
	if (drag_link->get_class() == "LinkerIndexGet") {
		return true;
	}
	return false;
}

void LinkerIndexCall::drop_data(Ref<LinkerLink> dropped_link) {
	if (dropped_link.is_null()) {
		return;
	}
	set_source(dropped_link);
}

LinkerLinkInstance *LinkerIndexCall::get_instance(LinkerScriptInstance *p_host, int p_stack_size) {
	if (!link_instances.has(p_stack_size)) {
		LinkerIndexCallInstance *instance = new LinkerIndexCallInstance();
		link_instances.insert(p_stack_size, p_host->add_link_instance(instance));
		_initialize_instance(link_instances[p_stack_size], p_host, p_stack_size);
	}
	return link_instances[p_stack_size];
}

void LinkerIndexCall::remove_instance(LinkerScriptInstance *p_host, int p_stack_size) {
	if (link_instances.has(p_stack_size)) {
		p_host->remove_link_instance(link_instances[p_stack_size]);
		link_instances.erase(p_stack_size);
	}
}

////////////////////////////////////////////////////////////////////////
//////////////////////////////  INSTANCE  //////////////////////////////
////////////////////////////////////////////////////////////////////////

int LinkerIndexCallInstance::step(StartMode p_start_mode, Callable::CallError &r_error, String &r_error_str) {
	return 0;
}
