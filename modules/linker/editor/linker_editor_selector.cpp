#include "linker_editor_selector.h"

void LinkerEditorSelector::inspect_links(const Array &p_links, const Vector2 &p_pos) {
	if (p_links.is_empty()) {
		return;
	}
	if (p_links[0].get_type() == Variant::INT) {
		Ref<LinkerLink> link = script->get_link(p_links[0]);
		if (link.is_valid()) {
			PropertyInfo pi = link->get_output_info();
			if (pi.type != Variant::NIL) {
				// editor selector for return type
			}
			// for now drop Print Method
			Ref<LinkerIndexCall> call;
			call.instantiate();
			call->set_index(StringName("print"));
			link->add_push_link_ref(call);
		}
	}
}
