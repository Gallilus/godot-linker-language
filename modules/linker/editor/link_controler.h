#ifndef LINK_CONTROLER_H
#define LINK_CONTROLER_H

#include "../language/linker_link.h"
#include "../language/linker_scene_refrence.h"

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

	virtual void _instantiate(); // use on_ready() and  update controle-SORT_CHILDREN
	virtual void update_connection();

	virtual void on_size_changed();

	Ref<Texture2D> _get_link_icon(LinkerLink *p_link) const;
	HorizontalAlignment _get_icon_h_alignement(LinkerLink *p_link) const;
	String _get_text(LinkerLink *p_link) const;

public:
	void set_link(Ref<LinkerLink> p_link);
	LinkerLink *get_link() const { return link.ptr(); }
	int get_link_idx() const { return link->get_link_idx(); }

	LinkControler();
	~LinkControler() {}
};

#endif // LINK_CONTROLER_H