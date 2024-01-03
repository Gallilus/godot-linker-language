#include "linker_link.h"
#include "linker_script.h"

void LinkerLink::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_link_idx"), &LinkerLink::get_link_idx);
	ClassDB::bind_method(D_METHOD("set_to_idx", "idx"), &LinkerLink::set_to_idx);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "links_idx"), "set_to_idx", "get_link_idx");

	ClassDB::bind_method(D_METHOD("get_index"), &LinkerLink::get_index);
	ClassDB::bind_method(D_METHOD("set_index", "idx"), &LinkerLink::set_index);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "index_name"), "set_index", "get_index");

	ClassDB::bind_method(D_METHOD("get_pull_links"), &LinkerLink::get_pull_links);
	ClassDB::bind_method(D_METHOD("set_pull_links", "idx"), &LinkerLink::set_pull_links);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "pull_links_idx "), "set_pull_links", "get_pull_links");

	ClassDB::bind_method(D_METHOD("get_push_links"), &LinkerLink::get_push_links);
	ClassDB::bind_method(D_METHOD("set_push_links", "idx"), &LinkerLink::set_push_links);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "push_links_idx"), "set_push_links", "get_push_links");

	ClassDB::bind_method(D_METHOD("get_owner_idx"), &LinkerLink::get_owner_idx);
	ClassDB::bind_method(D_METHOD("set_owner_idx", "idx"), &LinkerLink::set_owner_idx);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "owner_idx"), "set_owner_idx", "get_owner_idx");

	ADD_SIGNAL(MethodInfo("removed_from_script", PropertyInfo(Variant::BOOL, "force")));
}

void LinkerLink::_add_owned_link(Ref<LinkerLink> p_link, bool p_is_push) {
	if (!p_link.is_valid()) {
		return;
	}
	if (p_is_push) {
		push_links.append(p_link);
	} else {
		pull_links.append(p_link);
	}
	p_link->set_owner(this);
}

void LinkerLink::set_host(LinkerScript *p_host) {
	host = p_host;
	_set_owned_links();
}

void LinkerLink::set_source(Ref<LinkerLink> p_source) {
	if (pull_links.is_empty()) {
		pull_links.append(p_source);
		emit_signal("changed");
	} else {
		pull_links.set(0, p_source);
	}
}

void LinkerLink::set_owner(LinkerLink *p_link) {
	if (owner) {
		return;
	}
	if (p_link) {
		p_link->connect("removed_from_script", callable_mp(this, &LinkerLink::remove_from_script).bind(true));
	}
	owner = p_link;
}

int LinkerLink::get_link_idx() const {
	if (host == nullptr) {
		return -1;
	}
	return host->get_link_idx(this);
}

Array LinkerLink::get_pull_links() const {
	if (host == nullptr) {
		return Array();
	}
	Array r_idx;
	for (int i = 0; i < pull_links.size(); i++) {
		r_idx.append(pull_links[i]->get_link_idx());
	}
	return r_idx;
}

Array LinkerLink::get_push_links() const {
	if (host == nullptr) {
		return Array();
	}
	Array r_idx;
	for (int i = 0; i < push_links.size(); i++) {
		r_idx.append(push_links[i]->get_link_idx());
	}
	return r_idx;
}

int LinkerLink::get_owner_idx() const {
	if (owner) {
		return owner->get_link_idx();
	}
	return -1;
}

void LinkerLink::set_link_refrences() {
	for (int i = 0; i < pull_links_idx.size(); i++) {
		Ref<LinkerLink> link = get_host()->get_link(pull_links_idx[i]);
		if (link.is_valid()) {
			pull_links.append(link);
		}
	}
	for (int i = 0; i < push_links_idx.size(); i++) {
		Ref<LinkerLink> link = get_host()->get_link(push_links_idx[i]);
		if (link.is_valid()) {
			push_links.append(link);
		}
	}
	if (owner_links_idx != -1) {
		Ref<LinkerLink> link = get_host()->get_link(owner_links_idx);
		if (link.is_valid()) {
			set_owner(link.ptr());
		}
	}
}

PropertyInfo LinkerLink::get_output_info() {
	Dictionary info = get_placeholder_info();
	if (!info.has("type")) {
		return PropertyInfo();
	}
	if (info["type"] == "PropertyInfo") {
		return PropertyInfo::from_dict(info["value"]);
	}
	return PropertyInfo();
}

Variant LinkerLink::get_drag_data() const {
	Dictionary drag_data;
	drag_data["type"] = "linker_link";
	drag_data["link_idx"] = get_link_idx();
	// ToDo add script refrence
	return drag_data;
}

bool LinkerLink::can_drop(Ref<LinkerLink> drag_link) const {
	return false;
}

void LinkerLink::remove_from_script(bool p_force) {
	if (!p_force && owner) {
		return;
	}
	host->remove_link(this);
	emit_signal("removed_from_script");
}