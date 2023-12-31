#ifndef LINK_CONTROLER_H
#define LINK_CONTROLER_H

#include "../language/linker_link.h"
//#include "../language/linker_script.h"
//#include "linker_graph.h"

//#include "core/variant/dictionary.h"
#include "editor/editor_node.h"
#include "scene/gui/button.h"
#include "scene/gui/control.h"

class LinkControler : public Control {
	GDCLASS(LinkControler, Control);

	Ref<LinkerLink> link;

	Button *button = nullptr;

protected:
	static void _bind_methods() {}
	void _notification(int p_what);

	virtual void update_node(); // use on_ready() and  update controle-SORT_CHILDREN
	virtual void update_connection();

	virtual void on_size_changed();

	// get_inspector_options
	// get_option_data and option_functions (Seminar on godot tools)

	// handle getting dragged over
	// handle getting dragged

	// handle input (getting deleted, etc.)

public:
	void set_link(Ref<LinkerLink> p_link);
	LinkerLink *get_link() const { return link.ptr(); }
	int get_link_idx() const { return link->get_link_idx(); }

	virtual StringName get_category() const;

	LinkControler();
	~LinkControler() {}
};

#endif // LINK_CONTROLER_H