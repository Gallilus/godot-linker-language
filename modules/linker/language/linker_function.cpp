#include "linker_function.h"

void LinkerFunction::_bind_methods() {
}

void LinkerFunction::set_link_refrences() {
}

Vector<Ref<LinkerLink>> LinkerFunction::get_arg_links() {
	return Vector<Ref<LinkerLink>>();
}

int LinkerFunction::get_arg_count() const {
	return 0;
}

PropertyInfo LinkerFunction::get_arg_info(int p_idx) const {
	return PropertyInfo();
}

int LinkerFunction::get_default_arg_count() const {
	return 0;
}

Variant LinkerFunction::get_default_arg(int p_idx) const {
	return Variant();
}

int LinkerFunction::get_output_count() const {
	return 0;
}

PropertyInfo LinkerFunction::get_output_info(int p_idx) const {
	return PropertyInfo();
}

Ref<Texture2D> LinkerFunction::get_icon() const {
	return Ref<Texture2D>();
}
