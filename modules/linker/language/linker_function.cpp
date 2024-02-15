#include "linker_function.h"
#include "linker_index_call.h"
#include "linker_index_get.h"
#include "linker_script.h"

void LinkerFunction::_set_owned_links() {
	if (pull_links.size() == 0) {
		// only when using editor. so this needs to be triggerd on created by layout
		// Ref<LinkerIndexGet> return_link = memnew(LinkerIndexGet);
		// host->add_link(return_link);
		// add_push_link_ref(return_link);
	}
}

void LinkerFunction::_initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_stack_size) {
	LinkerFunctionInstance *instance = static_cast<LinkerFunctionInstance *>(link);

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

Variant LinkerFunction::get_placeholder_value() const {
	PropertyInfo info = host->get_method_info(index).return_val;
	return Variant();
}

Dictionary LinkerFunction::get_placeholder_info() const {
	// prefix dictionary with type MethodInfo
	// keys (type, value)
	return Dictionary(host->get_method_info(index).return_val);
}

bool LinkerFunction::can_drop_on_link(Ref<LinkerLink> drag_link) const {
	if (drag_link.is_null() ||
			drag_link == this) {
		return false;
	}
	return false;
}

LinkerLinkInstance *LinkerFunction::get_instance(LinkerScriptInstance *p_host, int p_stack_size) {
	if (!link_instances.has(p_stack_size)) {
		LinkerFunctionInstance *instance = new LinkerFunctionInstance();
		link_instances.insert(p_stack_size, p_host->add_link_instance(instance));
		_initialize_instance(link_instances[p_stack_size], p_host, p_stack_size);
	}
	return link_instances[p_stack_size];
}

void LinkerFunction::remove_instance(LinkerScriptInstance *p_host, int p_stack_size) {
	if (link_instances.has(p_stack_size)) {
		// need to remove the instance sublinks
		p_host->remove_link_instance(link_instances[p_stack_size]);
		link_instances.erase(p_stack_size);
	}
}

////////////////////////////////////////////////////////////////////////
//////////////////////////////  INSTANCE  //////////////////////////////
////////////////////////////////////////////////////////////////////////

int LinkerFunctionInstance::_step(StartMode p_start_mode, Callable::CallError &r_error, String &r_error_str) {
	if (pull_count > 0) {
		value = pull_links[0]->get_value();
	}
	return STEP_COMPLETE;
}