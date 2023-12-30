#ifndef LINK_CONTROLER_H
#define LINK_CONTROLER_H

#include "../language/linker_link.h"
#include "link_refrence.h"
//#include "../language/linker_script.h"
//#include "linker_graph.h"

//#include "core/variant/dictionary.h"
#include "editor/editor_node.h"
#include "editor/editor_string_names.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/control.h"
#include "scene/gui/label.h"
#include "scene/gui/margin_container.h"
#include "scene/gui/texture_rect.h"


class LinkControler : public MarginContainer {
	GDCLASS(LinkControler, MarginContainer);

	Ref<LinkerLink> link;

	Button *button = nullptr;
	HBoxContainer *title_box = nullptr;
	Label *title = nullptr;
	TextureRect *title_icon = nullptr;
	VBoxContainer *argument_refrences = nullptr;
	VBoxContainer *output_refrences = nullptr;

protected:
	static void _bind_methods() {}
	void _notification(int p_what);

	void _instantiate();
	void _instantiate_titlebox();

	virtual void update_node(); // use on_ready() and  update controle-SORT_CHILDREN
	virtual void update_connection();

	virtual void on_size_changed();

	// get_inspector_options
	// get_option_data and option_functions (Seminar on godot tools)

	// handle getting dragged over
	// handle getting dragged

	// handle input (getting deleted, etc.)

public:
	virtual Variant get_drag_data(const Point2 &p_point) override;
	virtual StringName get_category() const;

	void set_link(Ref<LinkerLink> p_link);
	LinkerLink *get_link() const;
	int get_link_idx() const { return link->get_link_idx(); }

	LinkControler();
	~LinkControler() {}
};

#endif // LINK_CONTROLER_H