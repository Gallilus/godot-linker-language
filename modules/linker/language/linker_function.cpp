#include "linker_function.h"
#include "linker_function_instance.h"
#include "linker_script.h"

void LinkerFunction::_set_owned_links() {
	// for every function argument argument without default value
	// create a LinkerGet
	// object and index will be set manually
}

void LinkerFunction::_initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_start_mode, int p_stack_size) {
	LinkerFunctionInstance *instance = static_cast<LinkerFunctionInstance *>(link);
	if (instance == nullptr) {
		ERR_PRINT("LinkerFunction::initialize_instance: instance is not a LinkerFunctionInstance");
		return;
	}
	instance->host = p_host;

	for (int i = 0; i < pull_links.size(); i++) {
		instance->pull_links.append(pull_links[i]->get_instance(p_host, p_start_mode, p_stack_size));
	}
	for (int i = 0; i < push_links.size(); i++) {
		instance->push_links.append(push_links[i]->get_instance(p_host, p_start_mode, p_stack_size));
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

LinkerLinkInstance *LinkerFunction::get_instance(LinkerScriptInstance *p_host, int p_start_mode, int p_stack_size) {
	if (!link_instances.has(p_stack_size)) {
		link_instances.insert(p_stack_size, p_host->add_link_instance(LinkerFunctionInstance()));
		_initialize_instance(link_instances[p_stack_size], p_host, p_start_mode, p_stack_size);
	}
	return link_instances[p_stack_size];
}
