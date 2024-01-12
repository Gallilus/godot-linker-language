#ifndef LINK_EDITOR_SELECTOR_H
#define LINK_EDITOR_SELECTOR_H

#include "../language/linker_script.h"
#include "../language/linker_index_call.h"

#include "scene/gui/control.h"

class LinkerEditorSelector : public Control {
	GDCLASS(LinkerEditorSelector, Control);
	Ref<LinkerScript> script;

protected:
	static void _bind_methods() {}

public:
	void set_script(Ref<LinkerScript> p_script) { script = p_script; }
	void inspect_links(const Array &p_links, const Vector2 &p_pos);

	LinkerEditorSelector() {}
	~LinkerEditorSelector() {}
};

#endif // LINK_EDITOR_SELECTOR_H