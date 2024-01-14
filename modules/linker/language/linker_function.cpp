#include "linker_function.h"
#include "linker_function_instance.h"
#include "linker_script.h"

void LinkerFunction::_set_owned_links() {
	// for every function argument argument without default value
	// create a LinkerGet
	// object and index will be set manually
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

void LinkerFunction::initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_start_mode, void *p_stack, int p_stack_size) {
	LinkerFunctionInstance *instance = static_cast<LinkerFunctionInstance *>(link);
	if (instance == nullptr) {
		ERR_PRINT("LinkerFunction::initialize_instance: instance is not a LinkerFunctionInstance");
		return;
	}
	instance->host = p_host;
	instance->index = index;
	


	ERR_PRINT("LinkerFunction::initialize_instance: not implemented");

	// instance->pull_links = pull_links;
	// instance->push_links = push_links;
}
