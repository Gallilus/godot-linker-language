#include "linker_index_get.h"

void LinkerIndexGet::_bind_methods() {}

Variant LinkerIndexGet::get_placeholder_value() const {
	// get the object index value
	return Variant();
}

Dictionary LinkerIndexGet::get_placeholder_info() const {
	// prefix dictionary with type PropertyInfo
	// keys (type, value)
	return Dictionary(/*get the object index info*/);
}

bool LinkerIndexGet::can_drop(Ref<LinkerLink> drag_link) const {
	if (drag_link.is_null()) {
		return false;
	}
	if (drag_link->get_class() == "LinkerIndexGet") {
		return true;
	}
	if (drag_link->get_class() == "LinkerIndexSet" &&
			drag_link != this) {
		return true;
	}
	return false;
}

void LinkerIndexGet::drop_data(Ref<LinkerLink> dropped_link) {
	if (dropped_link.is_null()) {
		return;
	}
	set_source(dropped_link);
}
