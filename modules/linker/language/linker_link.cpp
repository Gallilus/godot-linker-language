#include "linker_link.h"
#include "linker_script.h"

void LinkerLink::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_link_idx"), &LinkerLink::get_link_idx);
	ClassDB::bind_method(D_METHOD("set_to_idx", "idx"), &LinkerLink::set_to_idx);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "links_idx"), "set_to_idx", "get_link_idx");

	ADD_SIGNAL(MethodInfo("removed_from_script"));
}

void LinkerLink::set_host(LinkerScript *p_host) {
	host = p_host;
}

LinkerScript *LinkerLink::get_host() const {
	return host;
}

int LinkerLink::get_link_idx() const {
	if (host == nullptr) {
		return -1;
	}
	return host->get_link_idx(this);
}

void LinkerLink::set_to_idx(int p_idx) {
	saved_links_idx = p_idx;
}

void LinkerLink::remove_from_script() {
	host->remove_link(this);
	emit_signal("removed_from_script");
}

Ref<LinkerLink> LinkerLink::get_arg_link(int p_idx) const {
	if (p_idx < get_arg_count()) {
		ERR_PRINT("LinkerLink::get_arg_link: p_idx is out of range");
		return Ref<LinkerLink>();
	}

	if (argument_links.size() > p_idx) {
		return argument_links[p_idx];
	}
	return Ref<LinkerLink>();
}

void LinkerLink::set_arg_link(int p_idx, Ref<LinkerLink> p_link) {
	if (p_idx < get_arg_count()) {
		ERR_PRINT("LinkerLink::set_arg_link: p_idx is out of range");
	}
	if (argument_links.size() <= p_idx) {
		argument_links.resize(p_idx + 1);
	}
	argument_links.set(p_idx, p_link);
}

Ref<LinkerLink> LinkerLink::get_output_link(int p_idx) const {
	if (p_idx < get_output_count()) {
		ERR_PRINT("LinkerLink::get_output_link: p_idx is out of range");
		return Ref<LinkerLink>();
	}
	if (output_links.size() > p_idx) {
		return output_links[p_idx];
	}
	return Ref<LinkerLink>();
}

void LinkerLink::set_output_link(int p_idx, Ref<LinkerLink> p_link) {
	if (p_idx < get_output_count()) {
		ERR_PRINT("LinkerLink::set_output_link: p_idx is out of range");
	}
	if (output_links.size() <= p_idx) {
		output_links.resize(p_idx + 1);
	}
	output_links.set(p_idx, p_link);
}
