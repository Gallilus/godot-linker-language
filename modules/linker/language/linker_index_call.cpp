#include "linker_index_call.h"

bool LinkerIndexCall::can_drop(Ref<LinkerLink> drag_link) const {
	if (drag_link.is_null()) {
		return false;
	}
	if (drag_link->get_class() == "LinkerIndexGet") {
		return true;
	}
	return false;
}

void LinkerIndexCall::drop_data(Ref<LinkerLink> dropped_link) {
	if (dropped_link.is_null()) {
		return;
	}
	set_source(dropped_link);
}


// if pull links has no source it is a script function.
// further pull links are the arguments of the function.

// dubble click to edit index