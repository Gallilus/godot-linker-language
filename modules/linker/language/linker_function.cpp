#include "linker_function.h"
#include "linker_script.h"

void LinkerFunction::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_member_name", "name"), &LinkerFunction::set_member_name);
	ClassDB::bind_method(D_METHOD("get_member_name"), &LinkerFunction::get_member_name);
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "function_name"), "set_member_name", "get_member_name");
}

void LinkerFunction::set_link_refrences() {
	if (source_links_idx == -1) {
		ERR_PRINT("LinkerIndexGet::set_link_refrences: source_links_idx is -1");
		return;
	}
	source = get_host()->get_link(source_links_idx);
}

Vector<Ref<LinkerLink>> LinkerFunction::get_arg_links() {
	if (source.is_valid()) {
		return Vector<Ref<LinkerLink>>({ source });
	}
	return Vector<Ref<LinkerLink>>();
}

int LinkerFunction::get_arg_count() const {
	return 1;
}

PropertyInfo LinkerFunction::get_arg_info(int p_idx) const {
	return host->get_method_info(member_name).return_val;
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
	Control control;
	return control.get_theme_icon(SNAME("MemberMethod"), EditorStringName(EditorIcons));
}

void LinkerFunction::set_source(const Ref<LinkerLink> &p_source) {
	source = p_source;
}

Ref<LinkerLink> LinkerFunction::get_source() const {
	return source;
}
