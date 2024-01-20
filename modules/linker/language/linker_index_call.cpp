#include "linker_index_call.h"

void LinkerIndexCall::_initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_stack_size) {
	LinkerIndexCallInstance *instance = static_cast<LinkerIndexCallInstance *>(link);

	instance->host = p_host;
	instance->index = index;

	instance->pull_count = pull_links.size();
	instance->push_count = push_links.size();

	instance->input_args = new const Variant *[instance->pull_count];

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

bool LinkerIndexCall::can_drop(Ref<LinkerLink> drag_link) const {
	if (drag_link.is_null() ||
			drag_link == this) {
		return false;
	}
	if (drag_link->get_class() == "LinkerIndexGet") {
		return true;
	}
	return false;
}

void LinkerIndexCall::drop_data(Ref<LinkerLink> dropped_link) {
	// momentarly only compatible with one argument
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

int LinkerIndexCallInstance::_step(StartMode p_start_mode, Callable::CallError &r_error, String &r_error_str) {
	ERR_PRINT("LinkerIndexCallInstance::step: " + String(index));

	for (int i = 0; i < pull_count; i++) {
		input_args[i] = pull_links[i]->get_value();
		ERR_PRINT("LinkerIndexCallInstance::step: input_args[" + itos(i) + "] = " + String(*input_args[i]));
	}

	host->callp(index, input_args, pull_count, r_error);

	if (r_error.error != Callable::CallError::CALL_OK) {
		return STEP_ERROR;
	}
	return STEP_COMPLETE;
}

LinkerIndexCallInstance::~LinkerIndexCallInstance() {
	delete[] input_args;
}
