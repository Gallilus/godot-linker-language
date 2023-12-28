#include "linker_function_arg_refrence.h"

void LinkerFunctionArgRefrence::_bind_methods() {
}

void LinkerFunctionArgRefrence::set_link_refrences() {
}

Vector<Ref<LinkerLink>> LinkerFunctionArgRefrence::get_arg_links() {
	// get source
	// get function
	// get funtion instance
	return Vector<Ref<LinkerLink>>();
}

int LinkerFunctionArgRefrence::get_arg_count() const {
	return 0;
}

PropertyInfo LinkerFunctionArgRefrence::get_arg_info(int p_idx) const {
	return PropertyInfo();
}

int LinkerFunctionArgRefrence::get_default_arg_count() const {
	return 1;
}

Variant LinkerFunctionArgRefrence::get_default_arg(int p_idx) const {
	return Variant();
}

int LinkerFunctionArgRefrence::get_output_count() const {
	return 1;
}

PropertyInfo LinkerFunctionArgRefrence::get_output_info(int p_idx) const {
	// get function default arg
	// get function arg_info
	return PropertyInfo();
}

Ref<Texture2D> LinkerFunctionArgRefrence::get_icon() const {
	return Ref<Texture2D>();
}
