#include "linker_function.h"
#include "linker_index_get.h"
#include "linker_script.h"

void LinkerFunction::_set_owned_links() {
	if (pull_links.size() == 0) {
		// only when using editor. so this needs to be triggerd on created by layout
		Ref<LinkerIndexGet> return_link = memnew(LinkerIndexGet);
		host->add_link(return_link);
		add_push_link_ref(return_link);
	}
}

void LinkerFunction::_initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_stack_size) {
	LinkerFunctionInstance *instance = static_cast<LinkerFunctionInstance *>(link);

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

Variant LinkerFunction::get_placeholder_value() const {
	PropertyInfo info = host->get_method_info(index).return_val;
	return Variant();
}

Dictionary LinkerFunction::get_placeholder_info() const {
	// prefix dictionary with type MethodInfo
	// keys (type, value)
	return Dictionary(host->get_method_info(index).return_val);
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
		p_host->remove_link_instance(link_instances[p_stack_size]);
		link_instances.erase(p_stack_size);
	}
}

////////////////////////////////////////////////////////////////////////
//////////////////////////////  INSTANCE  //////////////////////////////
////////////////////////////////////////////////////////////////////////

int LinkerFunctionInstance::step(StartMode p_start_mode, Callable::CallError &r_error, String &r_error_str) {
	// find arguement from sourcecall
	// run all pull links
	// run all push links
	// handle how to get returnvalue form script
	return 0;
}