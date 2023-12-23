#include "linker_index_get.h"

int LinkerIndexGet::_get_source_links_idx() const {
	if (get_host() == nullptr) {
		return -1;
	}
	return get_host()->get_link_idx(source.ptr());
}

void LinkerIndexGet::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_index", "string"), &LinkerIndexGet::set_index);
	ClassDB::bind_method(D_METHOD("get_index"), &LinkerIndexGet::get_index);
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "index"), "set_index", "get_index");

	ClassDB::bind_method(D_METHOD("set_source_links_idx", "idx"), &LinkerIndexGet::_set_source_links_idx);
	ClassDB::bind_method(D_METHOD("get_source_links_idx"), &LinkerIndexGet::_get_source_links_idx);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "source_links_idx"), "set_source_links_idx", "get_source_links_idx");
}

void LinkerIndexGet::set_link_refrences() {
	if (source_links_idx == -1) {
		ERR_PRINT("LinkerIndexGet::set_link_refrences: source_links_idx is -1");
		return;
	}
	source = get_host()->get_link(source_links_idx);
}

int LinkerIndexGet::get_arg_count() const {
	return 0;
}
PropertyInfo LinkerIndexGet::get_arg_info(int p_idx) const {
	return PropertyInfo();
}

int LinkerIndexGet::get_default_arg_count() const {
	return 0;
}

Variant LinkerIndexGet::get_default_arg(int p_idx) const {
	return Variant();
}

PropertyInfo LinkerIndexGet::get_output_info(int p_idx) const {
	if (!source.is_valid()) {
		ERR_PRINT("LinkerIndexGet::get_output_info: source is not valid");
		return PropertyInfo();
	}
	PropertyInfo source_info = source->get_output_info(0);
	PropertyInfo return_info;
	ClassDB::get_property_info(source_info.class_name, index, &return_info);
	return return_info;
}

Ref<Texture2D> LinkerIndexGet::get_icon() const {
	PropertyInfo source_info = get_output_info(0);
	if (source_info.class_name == "") {
		return EditorNode::get_singleton()->get_class_icon(Variant::get_type_name(source_info.type));
	}
	return EditorNode::get_singleton()->get_class_icon(source_info.class_name);
}

void LinkerIndexGet::set_index(const StringName &p_index) {
	index = p_index;
}

StringName LinkerIndexGet::get_index() const {
	return index;
}

void LinkerIndexGet::set_source(const Ref<LinkerLink> &p_source) {
	source = p_source;
}

Ref<LinkerLink> LinkerIndexGet::get_source() const {
	return source;
}

Vector<Ref<LinkerLink>> LinkerIndexGet::get_arg_links() {
	if (source.is_valid()) {
		return Vector<Ref<LinkerLink>>({ source });
	}
	ERR_PRINT("LinkerIndexGet::get_arg_links: source is not valid");
	return Vector<Ref<LinkerLink>>();
}