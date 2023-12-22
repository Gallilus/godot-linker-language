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
