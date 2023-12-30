#ifndef LINK_REFRENCE_H
#define LINK_REFRENCE_H

#include "../language/linker_link.h"
#include "link_controler.h"

#include "editor/editor_string_names.h"
#include "scene/gui/control.h"
#include "editor/editor_node.h"
#include "scene/gui/margin_container.h"


class LinkRefrence : public MarginContainer {
	GDCLASS(LinkRefrence, MarginContainer);

private:
	Ref<LinkerLink> host;
	Ref<LinkerLink> refrence;
	int link_idx = -1;
	bool is_input = false;
	int refrence_idx = -1;
	PropertyInfo info;

	String label_override = "";

protected:
	static void _bind_methods() {}
	void _notification(int p_what);

	void _instantiate();

public:
	virtual Variant get_drag_data(const Point2 &p_point) override;

	void set_refrence(Ref<LinkerLink> p_refrence) { refrence = p_refrence; }
	void override_label(String p_label);

	LinkRefrence(int p_idx, bool p_is_input = true);
	~LinkRefrence() {}
};

#endif // LINK_REFRENCE_H