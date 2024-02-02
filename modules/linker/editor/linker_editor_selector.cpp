#include "linker_editor_selector.h"

// void LinkerEditorSelector::_popup(Vector2 p_pos) {
// 	set_position(p_pos);
// 	set_visible(true);
// 	set_size(Vector2(200, 200));
// 	set_position(Vector2(200, 2000));
// }

void LinkerEditorSelector::inspect_links(const Array &p_links, const Vector2 &p_pos) {
	set_title(TTR("select next"));
	if (p_links.is_empty()) {
		return;
	}
	if (p_links.size() == 1) {
		// assume link is source/destination.
		// search memebers
		// editor selector for multiple links
		return;
	}

	if (p_links[0].get_type() == Variant::INT) {
		Ref<LinkerLink> link = script->get_link(p_links[0]);
		if (link.is_valid()) {
			PropertyInfo pi = link->get_output_info();
			if (pi.type != Variant::NIL) {
				// editor selector for return type
			}

			// _popup(p_pos);
			popup_centered();
			//set_position(p_pos);

			// // for now drop Print Method
			// Ref<LinkerIndexCall> call;
			// call.instantiate();
			// call->set_index(StringName("print"));
			// link->add_push_link_ref(call);
		}
	}
}

LinkerEditorSelector::LinkerEditorSelector() {
	set_visible(false);
	set_ok_button_text(TTR("Open"));
	get_ok_button()->set_disabled(true);
	// set_hide_on_ok(false);
}
