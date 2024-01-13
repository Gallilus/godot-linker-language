#include "linker_function.h"
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

LinkerLinkInstance *LinkerFunction::create_instance() {
	return nullptr;
}
